#include <imgui.h>
#include <bave/imgui/im_text.hpp>
#include <bave/platform.hpp>
#include <spaced/game/player.hpp>

// temp for testing
#include <spaced/game/weapons/gun_beam.hpp>
#include <spaced/game/weapons/gun_kinetic.hpp>

namespace spaced {
using bave::im_text;
using bave::NotNull;
using bave::PointerMove;
using bave::PointerTap;
using bave::Seconds;
using bave::Shader;

Player::Player(Services const& services) : m_services(&services), m_controller(services) {
	auto const x = services.get<ILayout>().get_player_x();
	ship.transform.position.x = x;
	ship.set_auto_size(glm::vec2{100.0f});

	if constexpr (bave::platform_v == bave::Platform::eAndroid) { m_controller.set_type(Controller::Type::eTouch); }

	debug_switch_weapon();
}

void Player::on_focus(bave::FocusChange const& /*focus_changed*/) { m_controller.untap(); }

void Player::on_move(PointerMove const& pointer_move) { m_controller.on_move(pointer_move); }

void Player::on_tap(PointerTap const& pointer_tap) { m_controller.on_tap(pointer_tap); }

void Player::tick(std::span<NotNull<IDamageable*> const> targets, Seconds const dt) {
	auto const y_position = m_controller.tick(dt);
	set_y(y_position);

	ship.tick(dt);

	auto const muzzle_position = ship.transform.position + 0.5f * glm::vec2{ship.get_size().x, 0.0f};
	if (m_controller.is_firing() && m_debug.shots_remaining > 0) {
		if (auto round = m_weapon->fire(muzzle_position)) {
			m_weapon_rounds.push_back(std::move(round));
			--m_debug.shots_remaining;
		}
	}

	auto const round_state = IWeaponRound::State{.targets = targets, .muzzle_position = muzzle_position};
	for (auto const& round : m_weapon_rounds) { round->tick(round_state, dt); }
	std::erase_if(m_weapon_rounds, [](auto const& charge) { return charge->is_destroyed(); });

	m_weapon->tick(dt);

	if (m_debug.shots_remaining <= 0) { debug_switch_weapon(); }
}

void Player::draw(Shader& shader) const {
	ship.draw(shader);

	for (auto const& round : m_weapon_rounds) { round->draw(shader); }
}

void Player::set_y(float const y) { ship.transform.position.y = y; }

void Player::do_inspect() {
	if constexpr (bave::imgui_v) {
		if (ImGui::TreeNodeEx("Controller", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			m_controller.inspect();
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

void Player::debug_switch_weapon() {
	if (m_weapon && !m_weapon->is_idle()) { return; }

	if (dynamic_cast<GunKinetic const*>(m_weapon.get()) != nullptr) {
		m_weapon = std::make_unique<GunBeam>(*m_services);
		m_debug.shots_remaining = 2;
		return;
	}

	auto kinetic = std::make_unique<GunKinetic>(*m_services);
	kinetic->projectile_config.tint = bave::cyan_v;
	m_weapon = std::move(kinetic);
	m_debug.shots_remaining = 10;
}
} // namespace spaced
