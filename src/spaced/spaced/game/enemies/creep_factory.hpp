#pragma once
#include <spaced/game/enemy_factory.hpp>

namespace spaced {
class CreepFactory : public EnemyFactory {
  public:
	using EnemyFactory::EnemyFactory;

	[[nodiscard]] auto spawn_enemy() -> std::unique_ptr<Enemy> final;
};
} // namespace spaced
