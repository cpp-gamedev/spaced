#pragma once
#include <spaced/game/enemy.hpp>

namespace spaced {
class Creep : public Enemy {
  public:
	explicit Creep(Services const& services, bave::NotNull<IEnemyDeathListener*> listener) : Enemy(services, listener, "Creep") {}

	void tick(bave::Seconds dt) override;

	float x_speed{100.0f};
};
} // namespace spaced
