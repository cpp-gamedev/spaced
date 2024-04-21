#pragma once
#include <djson/json.hpp>
#include <spaced/game/enemy_factory.hpp>

namespace spaced {
class CreepFactory : public EnemyFactory {
  public:
	using EnemyFactory::EnemyFactory;

	[[nodiscard]] auto spawn_enemy() -> std::unique_ptr<Enemy> final;

  private:
	std::array<std::string_view, 2> m_tints{"orange", "milk"};
	float m_initial_health{2.0f};
};
} // namespace spaced
