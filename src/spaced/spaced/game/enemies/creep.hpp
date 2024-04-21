#pragma once
#include <spaced/game/enemy.hpp>

namespace spaced {
class Creep : public Enemy {
  public:
	explicit Creep(Services const& services, bave::NotNull<IEnemyDeathListener*> listener) : Enemy(services, listener, "Creep") {}

	void tick(bave::Seconds dt, bool in_play) override;

	float x_speed{100.0f};
};

class Creep2 : public Enemy2 {
  public:
	explicit Creep2(Services const& services) : Enemy2(services, "Creep") {}

	void tick(bave::Seconds dt, bool in_play) override;

	float x_speed{100.0f};
};
} // namespace spaced
