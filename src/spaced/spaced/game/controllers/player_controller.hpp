#pragma once
#include <bave/core/ptr.hpp>
#include <bave/input/gamepad.hpp>
#include <spaced/game/controllers/follow_controller.hpp>
#include <spaced/game/spring_arm.hpp>
#include <spaced/services/gamepad_provider.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>

namespace spaced {
class PlayerController : public FollowController {
  public:
	// mouse: single pointer drives motion and firing.
	// touch: left pointer for motion, right pointer for firing.
	enum class Type { eMouse, eTouch, eCOUNT_ };

	explicit PlayerController(Services const& services);

	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;

	void untap() final;

	[[nodiscard]] auto is_firing() const -> bool final { return m_fire_pointer.has_value() || m_fire_button; }
	void stop_firing() final;

	[[nodiscard]] auto get_type() const -> Type { return m_type; }
	void set_type(Type type);

	float max_y{};
	float min_y{};
	float n_move_area{0.25f}; // from left

	float gamepad_sensitivity{2000.0f};

  private:
	[[nodiscard]] auto is_in_move_area(glm::vec2 position) const -> bool;

	void tick_gamepad(bave::Seconds dt);

	auto tick_y(bave::Seconds dt) -> float final;
	void do_inspect() final;

	bave::Ptr<ILayout const> m_layout{};
	bave::Ptr<IGamepadProvider const> m_gamepad_provider{};

	Type m_type{};
	float m_y{};
	std::optional<bave::PointerId> m_fire_pointer{};
	bool m_fire_button{};
	bave::CString m_gamepad_name{};
};
} // namespace spaced
