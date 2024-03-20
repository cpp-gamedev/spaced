#pragma once
#include <bave/core/polymorphic.hpp>
#include <bave/graphics/rect.hpp>

namespace spaced {
class Player;

class IPowerup : public bave::Polymorphic {
  public:
	[[nodiscard]] virtual auto get_bounds() const -> bave::Rect<> = 0;
	virtual void activate(Player& player) = 0;
};
} // namespace spaced
