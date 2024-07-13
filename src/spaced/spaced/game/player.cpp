#include <imgui.h>
#include <bave/imgui/im_text.hpp>
#include <bave/platform.hpp>
#include <bave/services/resources.hpp>
#include <bave/services/styles.hpp>
#include <spaced/game/player.hpp>
#include <spaced/services/game_signals.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/stats.hpp>

// temp for testing
#include <spaced/game/weapons/gun_beam.hpp>

namespace spaced {
using bave::IAudio;
using bave::ParticleEmitter;
using bave::PointerMove;
using bave::PointerTap;
using bave::Rect;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Shader;
using bave::Styles;
using bave::Texture;

Player::Player(Services const& services, std::unique_ptr<IController> controller)
	: m_services(&services), m_audio(&services.get<IAudio>()), m_stats(&services.get<Stats>()), m_controller(std::move(controller)),
	  m_on_1up(&services.get<GameSignals>().one_up), m_shield(services), m_arsenal(services) {
	auto const& layout = services.get<Layout>();
	ship.transform.position.x = layout.player_x;

	auto const& resources = services.get<Resources>();
	auto const& rgbas = services.get<Styles>().rgbas;

	if (auto const texture = services.get<Resources>().get<Texture>("assets/images/ship_player.png")) {
		ship.set_texture(texture);
		ship.set_size(texture->get_size());
	}

	if (auto const exhaust = resources.get<ParticleEmitter>("assets/particles/exhaust.json")) { m_exhaust = *exhaust; }
	m_exhaust.config.lerp.tint.lo = rgbas["exhaust"];
	m_exhaust.set_position(get_exhaust_position());
	m_exhaust.config.respawn = true;
	m_exhaust.pre_warm();

	if (auto const death = resources.get<ParticleEmitter>("assets/particles/explode.json")) { m_death_source = *death; }
	m_death_source.config.respawn = false;

	m_heat_color = rgbas["ship_heat"];
}

void Player::on_focus(bave::FocusChange const& /*focus_changed*/) { m_controller->untap(); }

void Player::on_move(PointerMove const& pointer_move) { m_controller->on_move(pointer_move); }

void Player::on_tap(PointerTap const& pointer_tap) { m_controller->on_tap(pointer_tap); }

void Player::tick(State const& state, Seconds const dt) {
	if (m_death) {
		m_death->tick(dt);
		if (m_death->active_particles() == 0) { m_death.reset(); }
	}

	auto const round_state = IWeaponRound::State{
		.targets = state.targets,
		.muzzle_position = get_muzzle_position(),
		.in_play = !m_health.is_dead(),
	};

	auto const has_fired = m_arsenal.tick(round_state, m_controller->is_firing() && !m_is_cooling_down, dt);

	if (has_fired) {
		m_heat += m_heat_increment;
	} else {
		m_heat -= (m_is_cooling_down ? m_heat_dissipated * 0.5f : m_heat_dissipated) * dt.count();
	}
	if (m_heat >= 1.0f) { m_is_cooling_down = true; }
	if (m_heat <= m_cooldown_threshold) { m_is_cooling_down = false; }
	m_heat = std::clamp(m_heat, 0.f, 1.0f);

	m_heat_being_rendered = glm::mix(m_heat_being_rendered, m_heat, 0.5f);
	ship.tint = bave::Rgba::from(glm::mix(bave::white_v.to_vec4(), m_heat_color.to_vec4(), m_heat_being_rendered));

	m_shield.set_position(ship.transform.position);
	m_shield.tick(dt);

	m_exhaust.tick(dt);

	if (m_health.is_dead()) { return; }

	auto const y_position = m_controller->tick(dt);
	set_y(y_position);

	m_exhaust.set_position(get_exhaust_position());

	auto const bounds = get_bounds();
	for (auto const& target : state.targets) { check_hit(*target, bounds, dt); }

	for (auto const& powerup : state.powerups) {
		if (is_intersecting(powerup->get_bounds(), ship.get_bounds())) {
			powerup->activate(*this);
			++m_stats->player.powerups_collected;
		}
	}
}

void Player::draw(Shader& shader) const {
	m_exhaust.draw(shader);
	if (!m_health.is_dead()) {
		ship.draw(shader);
		m_shield.draw(shader);
	}
	m_arsenal.draw(shader);
	if (m_death) { m_death->draw(shader); }
}

auto Player::get_bounds() const -> Rect<> {
	if (m_shield.is_active()) { return m_shield.get_bounds(); }
	return bave::Rect<>::from_size(hitbox_size, ship.transform.position);
}

auto Player::take_damage(float damage) -> bool {
	if (is_dead()) { return false; }
	if (!m_shield.is_active()) {
		m_health.inflict_damage(damage);
		if (m_health.is_dead()) { on_death({}); }
	}
	return true;
}

void Player::force_death() { on_death({}); }

void Player::set_y(float const y) { ship.transform.position.y = y; }

auto Player::get_muzzle_position() const -> glm::vec2 { return ship.transform.position + 0.5f * glm::vec2{ship.get_shape().size.x, 0.0f}; }

auto Player::get_exhaust_position() const -> glm::vec2 { return ship.transform.position - 0.5f * glm::vec2{ship.get_shape().size.x, 0.0f}; }

void Player::set_controller(std::unique_ptr<IController> controller) {
	if (!controller) { return; }
	m_controller = std::move(controller);
}

void Player::set_shield(Seconds const ttl) {
	m_shield.ttl = ttl;
	m_shield.set_position(ship.transform.position);
}

void Player::one_up() { m_on_1up->dispatch(); }

void Player::on_death(Seconds const dt) {
	m_audio->play_sfx("assets/sfx/crunch.wav");

	m_health = 0.0f;
	m_death = m_death_source;
	m_death->set_position(ship.transform.position);
	m_death->tick(dt);

	m_exhaust.config.respawn = false;

	++m_stats->player.death_count;
}

auto Player::check_hit(IDamageable& out, Rect<> const& hitbox, Seconds const dt) -> bool {
	if (m_shield.is_active()) {
		if (is_intersecting(out.get_bounds(), m_shield.get_bounds())) { out.force_death(); }
		return false;
	}

	if (is_intersecting(out.get_bounds(), hitbox)) {
		out.force_death();
		on_death(dt);
		return true;
	}

	return false;
}

void Player::do_inspect() {
	if constexpr (bave::imgui_v) {
		if (ImGui::TreeNodeEx("Controller", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			m_controller->inspect();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Weapon", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button("Switch to Beam")) {
				auto beam = std::make_unique<GunBeam>(*m_services);
				beam->rounds = 2;
				m_arsenal.set_special(std::move(beam));
			}
			m_arsenal.get_weapon().inspect();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Shield", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			auto ttl = m_shield.ttl.count();
			if (ImGui::DragFloat("ttl", &ttl, 0.25f, 0.0f, 600.0f, "%.2f")) { m_shield.ttl = Seconds{ttl}; }
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Status", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			m_health.inspect();
			ImGui::TreePop();
		}

		if (ImGui::Button("1up")) { one_up(); }

		ImGui::DragFloat("heat dissipated per sec", &m_heat_dissipated, 0.05f, 0.f, 1.f);
		ImGui::DragFloat("heat per round fired", &m_heat_increment, 0.1f, 0.f, 1.f);
		ImGui::DragFloat("cooldown threshold", &m_cooldown_threshold, 0.1f, 0.f, 0.9f);
	}
}
} // namespace spaced
