#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/drawable.hpp>
#include <spaced/game/damageable.hpp>
#include <span>

namespace spaced {
class IWeaponRound : public bave::IDrawable {
  public:
	struct State {
		std::span<bave::NotNull<IDamageable*> const> targets{};
		glm::vec2 muzzle_position{};
		bool in_play{true};
	};

	virtual void tick(State const& state, bave::Seconds dt) = 0;
	[[nodiscard]] virtual auto is_destroyed() const -> bool = 0;
};
} // namespace spaced
