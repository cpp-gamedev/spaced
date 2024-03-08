#pragma once
#include <spaced/game/enemy.hpp>

namespace spaced {
class Creep : public Enemy {
  public:
	explicit Creep(Services const& services, bave::NotNull<IScorer*> scorer) : Enemy(services, scorer, "Creep") {}

	void tick(bave::Seconds dt) override;

	float x_speed{100.0f};
};
} // namespace spaced
