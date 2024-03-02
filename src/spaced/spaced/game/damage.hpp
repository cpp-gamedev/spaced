#pragma once
#include <bave/app.hpp>
#include <bave/imgui/im_text.hpp>

namespace spaced {
class Damage {
  public:
	constexpr void inflict(int amount) { m_health -= glm::clamp(amount, amount, m_health); }
	constexpr bool is_dead() const { return m_health <= 0; }
	constexpr int get_health() const { return m_health; }

	void debug_stuff() {
		if (ImGui::TreeNode("damage")) {
			bave::im_text("health: {}", m_health);
			bave::im_text("dead: {}", is_dead() ? "yes" : "no");
			bool inflict_ten{};
			ImGui::Checkbox("inflict 10 damage", &inflict_ten);
			if (inflict_ten) { inflict(10); }
			ImGui::SliderInt("health", &m_health, 0, 100);
			ImGui::TreePop();
		}
	}

  private:
	int m_health{25};
};
} // namespace spaced
