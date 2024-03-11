#pragma once
#include <bave/graphics/particle_system.hpp>
#include <spaced/game/enemy.hpp>
#include <functional>

namespace spaced {
class EnemySpawner {
  public:
	using Spawn = std::function<std::unique_ptr<Enemy>()>;

	explicit EnemySpawner(Spawn spawn, bave::ParticleEmitter explode);

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const;

	void spawn() { m_enemies.push_back(m_spawn()); }

	[[nodiscard]] auto get_enemies() const -> std::span<std::unique_ptr<Enemy> const> { return m_enemies; }
	void append_targets(std::vector<bave::NotNull<IDamageable*>>& out) const;

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

  private:
	void explode_at(glm::vec2 position);
	void do_inspect();

	Spawn m_spawn{};
	bave::ParticleEmitter m_explode{};
	std::vector<std::unique_ptr<Enemy>> m_enemies{};
	std::vector<bave::ParticleEmitter> m_explodes{};
};
} // namespace spaced
