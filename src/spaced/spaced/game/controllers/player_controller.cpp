#include <imgui.h>
#include <bave/imgui/im_text.hpp>
#include <bave/platform.hpp>
#include <spaced/game/controllers/player_controller.hpp>
#include <algorithm>

namespace spaced {
using bave::Action;
using bave::EnumArray;
using bave::GamepadAxis;
using bave::GamepadButton;
using bave::im_text;
using bave::PointerMove;
using bave::PointerTap;
using bave::Seconds;

PlayerController::PlayerController(Services const& services) : m_layout(&services.get<ILayout>()), m_gamepad_provider(&services.get<IGamepadProvider>()) {
	max_y = 0.5f * m_layout->get_world_space().y;
	min_y = -max_y; // NOLINT(cppcoreguidelines-prefer-member-initializer)
}

void PlayerController::on_move(PointerMove const& pointer_move) {
	auto const world_pos = m_layout->project_to_world(pointer_move.pointer.position);

	if (m_type == Type::eTouch) {
		if (!is_in_move_area(world_pos)) {
			// pointer motion outside move area is ignored
			return;
		}

		if (m_fire_pointer && pointer_move.pointer.id == *m_fire_pointer) {
			// fire pointer has moved into move-area, reset firing
			m_fire_pointer.reset();
			return;
		}
	}

	m_y = world_pos.y;
}

void PlayerController::on_tap(PointerTap const& pointer_tap) {
	auto const world_pos = m_layout->project_to_world(pointer_tap.pointer.position);
	if (m_type == Type::eTouch && is_in_move_area(world_pos)) {
		// pointer tap in move area is ingored
		return;
	}

	// track firing pointer
	switch (pointer_tap.action) {
	case Action::ePress: m_fire_pointer = pointer_tap.pointer.id; break;
	case Action::eRelease: m_fire_pointer.reset(); break;
	default: break;
	}
}

void PlayerController::untap() { m_fire_pointer.reset(); }

void PlayerController::set_type(Type const type) {
	untap();
	m_type = type;
}

void PlayerController::stop_firing() {
	m_fire_button = false;
	m_fire_pointer.reset();
}

auto PlayerController::tick(Seconds const dt) -> float {
	if (m_reload_remain > 0s) { m_reload_remain -= dt; }

	tick_gamepad(dt);
	m_y = std::clamp(m_y, min_y, max_y);

	m_spring_arm.target = glm::vec2{0.0f, m_y};
	m_spring_arm.tick(dt);

	return m_spring_arm.position.y;
}

void PlayerController::do_inspect() {
	if constexpr (bave::imgui_v) {
		bool firing = m_fire_pointer.has_value();
		ImGui::BeginDisabled();
		ImGui::Checkbox("firing", &firing);
		ImGui::EndDisabled();

		static constexpr auto type_names_v = bave::EnumArray<Type, std::string_view>{
			"mouse",
			"touch",
		};
		auto const type_name = type_names_v[get_type()];
		if (ImGui::BeginCombo("controller type", type_name.data())) {
			for (std::size_t i = 0; i < type_names_v.size(); ++i) {
				auto const type = static_cast<Type>(i);
				auto const name = type_names_v.at(type);
				if (ImGui::Selectable(name.data())) { set_type(type); }
			}
			ImGui::EndCombo();
		}

		auto const gamepad_name = m_gamepad_name.as_view();
		im_text("gamepad: {}", gamepad_name.empty() ? "[none]" : gamepad_name);

		if (ImGui::TreeNode("spring arm")) {
			ImGui::DragFloat("k", &m_spring_arm.k, 1.0f, 1.0f, 1000.0f);
			ImGui::DragFloat("damp", &m_spring_arm.damp, 0.01f, 0.01f, 0.99f);
			ImGui::DragFloat("min distance", &m_spring_arm.min_distance, 0.01f, 0.01f, 1000.0f);
			ImGui::TreePop();
		}
	}
}

auto PlayerController::is_in_move_area(glm::vec2 const position) const -> bool {
	auto const n_pos = position.x / m_layout->get_world_space().x;
	return n_pos <= -n_move_area;
}

void PlayerController::tick_gamepad(Seconds const dt) {
	auto const& gamepad = m_gamepad_provider->get_gamepad();
	if (!gamepad.connected) {
		m_gamepad_name = {};
		return;
	}

	m_y += gamepad.get_axis(GamepadAxis::eLeftY) * gamepad_sensitivity * dt.count();
	m_fire_button = gamepad.is_pressed(GamepadButton::eX);
	m_gamepad_name = gamepad.name;
}
} // namespace spaced
