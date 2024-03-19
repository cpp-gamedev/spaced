#pragma once
#include <bave/graphics/particle_system.hpp>
#include <spaced/game/enemy_factory.hpp>

namespace spaced {
class EnemySpawner {
  public:
	explicit EnemySpawner(std::unique_ptr<IEnemyFactory> factory);

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const;

	void spawn() { m_enemies.push_back(m_factory->spawn_enemy()); }

	[[nodiscard]] auto get_enemies() const -> std::span<std::unique_ptr<Enemy> const> { return m_enemies; }
	void append_targets(std::vector<bave::NotNull<IDamageable*>>& out) const;

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

  private:
	void explode_at(glm::vec2 position);
	void do_inspect();

	std::unique_ptr<IEnemyFactory> m_factory{};
	bave::ParticleEmitter m_death_emitter{};
	std::vector<std::unique_ptr<Enemy>> m_enemies{};
	std::vector<bave::ParticleEmitter> m_death_emitters{};
};
} // namespace spaced
