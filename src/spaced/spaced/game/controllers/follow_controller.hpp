#pragma once
#include <bave/core/inclusive_range.hpp>
#include <spaced/game/controller.hpp>
#include <spaced/game/spring_arm.hpp>

namespace spaced {
class FollowController : public IController {
  public:
	auto tick(bave::Seconds dt) -> float final;

  protected:
	virtual auto tick_y(bave::Seconds dt) -> float = 0;
	void do_inspect() override;

	SpringArm m_spring_arm{};
	bave::InclusiveRange<float> m_spring_k{200.0f, 40.0f};
};
} // namespace spaced
