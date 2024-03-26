#pragma once
#include <bave/core/polymorphic.hpp>
#include <bave/core/time.hpp>
#include <bave/graphics/drawable.hpp>
#include <bave/graphics/rect.hpp>

namespace spaced {
class Player;

class IPowerup : public bave::IDrawable {
  public:
	[[nodiscard]] virtual auto get_bounds() const -> bave::Rect<> = 0;
	virtual void activate(Player& player) = 0;

	[[nodiscard]] virtual auto is_destroyed() const -> bool = 0;

	virtual void tick(bave::Seconds dt) = 0;
};
} // namespace spaced
