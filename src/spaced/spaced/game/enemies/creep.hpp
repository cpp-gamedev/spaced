#pragma once
#include <spaced/game/enemy.hpp>

namespace spaced {
class Creep : public Enemy {
  public:
	explicit Creep(bave::Services const& services) : Enemy(services, "Creep") {}

	void tick(bave::Seconds dt, bool in_play) override;

	float x_speed{100.0f};
};
} // namespace spaced
