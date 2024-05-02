#pragma once
#include <bave/core/polymorphic.hpp>
#include <bave/graphics/rect.hpp>
#include <spaced/game/instigator.hpp>

namespace spaced {
class IDamageable : public bave::Polymorphic {
  public:
	[[nodiscard]] virtual auto get_instigator() const -> Instigator = 0;
	[[nodiscard]] virtual auto get_bounds() const -> bave::Rect<> = 0;
	virtual auto take_damage(float damage) -> bool = 0;
	virtual void force_death() = 0;
};
} // namespace spaced
