#pragma once
#include <spaced/game/enemy.hpp>

namespace spaced::enemy {
class Creep : public Enemy {
  public:
	explicit Creep(bave::Services const& services);
};
} // namespace spaced::enemy
