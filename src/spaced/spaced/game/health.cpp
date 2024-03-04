#include <imgui.h>
#include <bave/imgui/im_text.hpp>
#include <bave/platform.hpp>
#include <spaced/game/health.hpp>
#include <algorithm>

namespace spaced {
void Health::inflict_damage(float const amount) { m_health = std::clamp(m_health - amount, 0.0f, m_health); }

bool Health::is_dead() const { return m_health <= 0.f; }

float Health::get_health() const { return m_health; }

void Health::debug_stuff() {
	if constexpr (bave::imgui_v) {
		if (ImGui::TreeNode("health")) {
			bave::im_text("health: {}", m_health);
			bave::im_text("dead: {}", is_dead() ? "yes" : "no");
			if (ImGui::Button("inflict 10 damage")) { inflict_damage(10.f); }
			ImGui::SliderFloat("health", &m_health, 0.f, 100.f, "%.1f");
			ImGui::TreePop();
		}
	}
}
} // namespace spaced
