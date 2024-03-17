#pragma once
#include <bave/graphics/particle_system.hpp>
#include <spaced/game/enemy.hpp>

namespace spaced {
class IEnemyFactory : public bave::Polymorphic {
  public:
	[[nodiscard]] virtual auto get_type_name() const -> std::string_view = 0;
	[[nodiscard]] virtual auto spawn_enemy() -> std::unique_ptr<Enemy> = 0;
	[[nodiscard]] virtual auto get_death_emitter() const -> bave::ParticleEmitter const& = 0;
};
} // namespace spaced
