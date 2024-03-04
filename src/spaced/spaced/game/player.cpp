#include <imgui.h>
#include <bave/platform.hpp>
#include <spaced/game/player.hpp>

namespace spaced {
using bave::PointerMove;
using bave::PointerTap;
using bave::Seconds;
using bave::Shader;

Player::Player(Services const& services) : m_controller(services) {
	auto const x = services.get<ILayout>().get_player_x();
	ship.transform.position.x = x;

	if constexpr (bave::platform_v == bave::Platform::eAndroid) { m_controller.set_type(Controller::Type::eTouch); }
}

void Player::on_move(PointerMove const& pointer_move) { m_controller.on_move(pointer_move); }

void Player::on_tap(PointerTap const& pointer_tap) { m_controller.on_tap(pointer_tap); }

void Player::tick(Seconds const dt) {
	auto const state = m_controller.tick(dt);
	set_y(state.y_position);

	if (m_controller.is_firing()) {
		ship.tint = bave::red_v;
	} else {
		ship.tint = {};
	}

	ship.tick(dt);
}

void Player::draw(Shader& shader) const { ship.draw(shader); }

void Player::set_y(float const y) { ship.transform.position.y = y; }

void Player::debug_stuff() {
	if constexpr (bave::imgui_v) {
		if (ImGui::TreeNodeEx("Controller", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			m_controller.debug_stuff();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Status", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			health.debug_stuff();
			ImGui::TreePop();
		}
	}
}
} // namespace spaced
