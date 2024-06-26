#include <imgui.h>
#include <spaced/game/controllers/follow_controller.hpp>

namespace spaced {
using bave::Seconds;

auto FollowController::tick(Seconds const dt) -> float {
	auto const y = tick_y(dt);

	m_spring_arm.target = glm::vec2{0.0f, y};
	m_spring_arm.k = is_firing() ? m_spring_k.hi : m_spring_k.lo;
	m_spring_arm.tick(dt);

	return m_spring_arm.position.y;
}

void FollowController::do_inspect() {
	if constexpr (bave::imgui_v) {
		if (ImGui::TreeNode("spring arm")) {
			ImGui::DragFloat("k.lo", &m_spring_k.lo, 1.0f, 1.0f, 1000.0f);
			ImGui::DragFloat("k.hi", &m_spring_k.hi, 1.0f, 1.0f, 1000.0f);
			ImGui::DragFloat("k", &m_spring_arm.k, 1.0f, 1.0f, 1000.0f);
			ImGui::DragFloat("damp", &m_spring_arm.damp, 0.01f, 0.01f, 0.99f);
			ImGui::DragFloat("min distance", &m_spring_arm.min_distance, 0.01f, 0.01f, 1000.0f);
			ImGui::TreePop();
		}
	}
}
} // namespace spaced
