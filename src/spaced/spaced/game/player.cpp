#include <imgui.h>
#include <bave/imgui/im_text.hpp>
#include <bave/platform.hpp>
#include <spaced/game/player.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/stats.hpp>
#include <spaced/services/styles.hpp>

// temp for testing
#include <spaced/game/weapons/gun_beam.hpp>

namespace spaced {
using bave::ParticleEmitter;
using bave::PointerMove;
using bave::PointerTap;
using bave::RoundedQuad;
using bave::Seconds;
using bave::Shader;

Player::Player(Services const& services, std::unique_ptr<IController> controller)
	: m_services(&services), m_stats(&services.get<Stats>()), m_controller(std::move(controller)) {
	auto const& layout = services.get<ILayout>();
	ship.transform.position.x = layout.get_player_x();
	auto rounded_quad = RoundedQuad{};
	rounded_quad.size = layout.get_player_size();
	rounded_quad.corner_radius = 20.0f;
	ship.set_shape(rounded_quad);

	auto const& rgbas = services.get<Styles>().rgbas;
	ship.tint = rgbas["black"];

	auto const& resources = services.get<Resources>();
	if (auto const exhaust = resources.get<ParticleEmitter>("particles/exhaust.json")) { m_exhaust = *exhaust; }
	m_exhaust.set_position(get_exhaust_position());
	m_exhaust.pre_warm();

	if (auto const death = resources.get<ParticleEmitter>("particles/explode.json")) { m_death_source = *death; }
	m_death_source.config.respawn = false;
}

void Player::on_focus(bave::FocusChange const& /*focus_changed*/) { m_controller->untap(); }

void Player::on_move(PointerMove const& pointer_move) { m_controller->on_move(pointer_move); }

void Player::on_tap(PointerTap const& pointer_tap) { m_controller->on_tap(pointer_tap); }

void Player::tick(State const& state, Seconds const dt) {
	if (m_death) {
		m_death->tick(dt);
		if (m_death->active_particles() == 0) { m_death.reset(); }
	}

	if (health.is_dead()) { return; }

	auto const y_position = m_controller->tick(dt);
	set_y(y_position);

	for (auto const& target : state.targets) {
		if (is_intersecting(target->get_bounds(), ship.get_bounds())) {
			on_death(dt);
			target->force_death();
			return;
		}
	}

	auto const round_state = IWeaponRound::State{.targets = state.targets, .muzzle_position = get_muzzle_position()};
	m_arsenal.tick(round_state, m_controller->is_firing(), dt);

	m_exhaust.set_position(get_exhaust_position());
	m_exhaust.tick(dt);

	for (auto const& powerup : state.powerups) {
		if (is_intersecting(powerup->get_bounds(), ship.get_bounds())) {
			powerup->activate(*this);
			++m_stats->player.powerups_collected;
		}
	}
}

void Player::draw(Shader& shader) const {
	if (!health.is_dead()) {
		m_exhaust.draw(shader);
		ship.draw(shader);
	}
	m_arsenal.draw(shader);
	if (m_death) { m_death->draw(shader); }
}

void Player::set_y(float const y) { ship.transform.position.y = y; }

auto Player::get_muzzle_position() const -> glm::vec2 { return ship.transform.position + 0.5f * glm::vec2{ship.get_shape().size.x, 0.0f}; }

auto Player::get_exhaust_position() const -> glm::vec2 { return ship.transform.position - 0.5f * glm::vec2{ship.get_shape().size.x, 0.0f}; }

void Player::set_controller(std::unique_ptr<IController> controller) {
	if (!controller) { return; }
	m_controller = std::move(controller);
}

void Player::on_death(Seconds const dt) {
	health = 0.0f;
	m_death = m_death_source;
	m_death->set_position(ship.transform.position);
	m_death->tick(dt);
	++m_stats->player.death_count;
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
		if (ImGui::TreeNodeEx("Status", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			health.inspect();
			ImGui::TreePop();
		}
	}
}
} // namespace spaced
