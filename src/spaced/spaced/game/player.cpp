#include <imgui.h>
#include <bave/imgui/im_text.hpp>
#include <bave/platform.hpp>
#include <spaced/game/player.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/styles.hpp>

// temp for testing
#include <spaced/game/weapons/gun_beam.hpp>
#include <spaced/game/weapons/gun_kinetic.hpp>

namespace spaced {
using bave::im_text;
using bave::NotNull;
using bave::ParticleEmitter;
using bave::PointerMove;
using bave::PointerTap;
using bave::RoundedQuad;
using bave::Seconds;
using bave::Shader;

Player::Player(Services const& services, std::unique_ptr<IController> controller) : m_services(&services), m_controller(std::move(controller)) {
	setup_ship();

	debug_switch_weapon();
}

void Player::on_focus(bave::FocusChange const& /*focus_changed*/) { m_controller->untap(); }

void Player::on_move(PointerMove const& pointer_move) { m_controller->on_move(pointer_move); }

void Player::on_tap(PointerTap const& pointer_tap) { m_controller->on_tap(pointer_tap); }

void Player::tick(std::span<NotNull<IDamageable*> const> targets, Seconds const dt) {
	auto const y_position = m_controller->tick(dt);
	set_y(y_position);

	auto const muzzle_position = get_muzzle_position();
	if (m_controller->is_firing() && m_debug.shots_remaining > 0) {
		if (auto round = m_weapon->fire(muzzle_position)) {
			m_weapon_rounds.push_back(std::move(round));
			--m_debug.shots_remaining;
		}
	}

	auto const round_state = IWeaponRound::State{.targets = targets, .muzzle_position = muzzle_position};
	for (auto const& round : m_weapon_rounds) { round->tick(round_state, dt); }
	std::erase_if(m_weapon_rounds, [](auto const& charge) { return charge->is_destroyed(); });

	m_weapon->tick(dt);

	m_exhaust.set_position(get_exhaust_position());

	m_exhaust.tick(dt);

	if (m_debug.shots_remaining <= 0) { debug_switch_weapon(); }
}

void Player::draw(Shader& shader) const {
	m_exhaust.draw(shader);
	ship.draw(shader);

	for (auto const& round : m_weapon_rounds) { round->draw(shader); }
}

void Player::setup(WorldSpec::Player const& spec) {
	auto const& rgbas = m_services->get<Styles>().rgbas;
	auto const& resources = m_services->get<Resources>();
	ship.tint = rgbas[spec.tint];
	if (auto const exhaust = resources.get<ParticleEmitter>(spec.exhaust_emitter)) { m_exhaust = *exhaust; }
	m_exhaust.set_position(get_exhaust_position());
	m_exhaust.pre_warm();
}

void Player::set_y(float const y) { ship.transform.position.y = y; }

auto Player::get_muzzle_position() const -> glm::vec2 { return ship.transform.position + 0.5f * glm::vec2{ship.get_shape().size.x, 0.0f}; }

auto Player::get_exhaust_position() const -> glm::vec2 { return ship.transform.position - 0.5f * glm::vec2{ship.get_shape().size.x, 0.0f}; }

void Player::set_controller(std::unique_ptr<IController> controller) {
	if (!controller) { return; }
	m_controller = std::move(controller);
}

void Player::do_inspect() {
	if constexpr (bave::imgui_v) {
		if (ImGui::TreeNodeEx("Controller", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			m_controller->inspect();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Weapon", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			m_weapon->inspect();
			im_text("shots remaining: {}", m_debug.shots_remaining);
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Status", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			health.inspect();
			ImGui::TreePop();
		}
	}
}

void Player::setup_ship() {
	auto const& layout = m_services->get<ILayout>();
	ship.transform.position.x = layout.get_player_x();
	auto rounded_quad = RoundedQuad{};
	rounded_quad.size = layout.get_player_size();
	rounded_quad.corner_radius = 20.0f;
	ship.set_shape(rounded_quad);
}

void Player::debug_switch_weapon() {
	if (m_weapon && !m_weapon->is_idle()) { return; }

	if (dynamic_cast<GunKinetic const*>(m_weapon.get()) != nullptr) {
		m_weapon = std::make_unique<GunBeam>(*m_services);
		m_debug.shots_remaining = 2;
		return;
	}

	m_weapon = std::make_unique<GunKinetic>(*m_services);
	m_debug.shots_remaining = 10;
}
} // namespace spaced
