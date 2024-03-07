#include <imgui.h>
#include <spaced/game/controllers/auto_controller.hpp>
#include <cmath>

namespace spaced {
using bave::NotNull;
using bave::Ptr;
using bave::Rect;
using bave::Seconds;

namespace {
[[nodiscard]] auto nearest_target_y(std::span<NotNull<IDamageable*> const> targets, glm::vec2 const muzzle) -> std::optional<Rect<>> {
	auto ret = std::optional<Rect<>>{};
	auto min_distance = 0.0f;
	for (auto target : targets) {
		auto const bounds = target->get_bounds();
		if (bounds.top_left().x < muzzle.x) { continue; }
		auto const distance = std::abs(bounds.top_left().x - muzzle.x);
		if (distance < min_distance || !ret) {
			ret = bounds;
			min_distance = distance;
		}
	}
	return ret;
}
} // namespace

AutoController::AutoController(bave::NotNull<ITargetProvider const*> target_provider, float const muzzle_x)
	: m_target_provider(target_provider), m_muzzle_x(muzzle_x) {}

auto AutoController::tick_y(Seconds const dt) -> float {
	auto const target_y = nearest_target_y(m_target_provider->get_targets(), {m_muzzle_x, m_spring_arm.position.y});
	if (target_y) {
		m_target_y = target_y->centre().y;
		m_firing = m_spring_arm.position.y >= target_y->bottom_right().y && m_spring_arm.position.y <= target_y->top_left().y;
	} else {
		m_firing = false;
	}
	m_y = std::lerp(m_y, m_target_y, m_lerp_speed * dt.count());
	return m_y;
}

void AutoController::do_inspect() {
	if constexpr (bave::imgui_v) {
		bool firing = m_firing;
		ImGui::BeginDisabled();
		ImGui::Checkbox("firing", &firing);
		ImGui::EndDisabled();

		ImGui::DragFloat("lerp speed", &m_lerp_speed, 0.2f, 0.2f, 100.0f);

		FollowController::do_inspect();
	}
}
} // namespace spaced
