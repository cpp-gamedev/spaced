#pragma once
#include <bave/core/ptr.hpp>
#include <bave/input/gamepad.hpp>
#include <spaced/game/controllers/follow_controller.hpp>
#include <spaced/game/spring_arm.hpp>
#include <spaced/game/target_provider.hpp>
#include <spaced/services/gamepad_provider.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>

namespace spaced {
class AutoController : public FollowController {
  public:
	static constexpr std::string_view type_name_v{"AutoController"};

	[[nodiscard]] auto get_type_name() const -> std::string_view final { return type_name_v; };

	explicit AutoController(bave::NotNull<ITargetProvider const*> target_provider, float muzzle_x);

	void on_move(bave::PointerMove const& /*pointer_move*/) final {}
	void on_tap(bave::PointerTap const& /*pointer_tap*/) final {}

	void untap() final {}

	[[nodiscard]] auto is_firing() const -> bool final { return m_firing; }
	void stop_firing() final { m_firing = false; }

  private:
	auto tick_y(bave::Seconds dt) -> float final;
	void do_inspect() final;

	bave::NotNull<ITargetProvider const*> m_target_provider;
	float m_muzzle_x{};

	float m_target_y{};
	float m_y{};
	float m_lerp_speed{5.0f};
	bool m_firing{};
};
} // namespace spaced
