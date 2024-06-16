#pragma once
#include <spaced/game/enemy_factory.hpp>

namespace spaced {
class CreepFactory : public EnemyFactory {
  public:
	explicit CreepFactory(bave::NotNull<bave::Services const*> services);

	[[nodiscard]] auto spawn_enemy() -> std::unique_ptr<Enemy> final;

  private:
	std::shared_ptr<bave::Texture const> m_ship_texture{};
	float m_initial_health{2.0f};
};
} // namespace spaced
