#include <imgui.h>
#include <bave/imgui/im_text.hpp>
#include <bave/platform.hpp>
#include <spaced/game/health.hpp>
#include <algorithm>

namespace spaced {
void Health::inflict_damage(float const amount) { set_hit_points(m_hit_points - amount); }

bool Health::is_dead() const { return m_hit_points <= 0.f; }

float Health::get_hit_points() const { return m_hit_points; }

void Health::set_hit_points(float const hit_points) { m_hit_points = std::max(hit_points, 0.0f); }

void Health::do_inspect() {
	if constexpr (bave::imgui_v) {
		if (ImGui::TreeNode("health")) {
			bave::im_text("health: {:.2f}", m_hit_points);
			bave::im_text("dead: {}", is_dead() ? "yes" : "no");
			if (ImGui::Button("inflict 10 damage")) { inflict_damage(10.f); }
			ImGui::SliderFloat("health", &m_hit_points, 0.f, 100.f, "%.2f");
			ImGui::TreePop();
		}
	}
}
} // namespace spaced
