#include <spaced/game/enemy_spawner.hpp>

namespace spaced {
using bave::ParticleEmitter;
using bave::Seconds;
using bave::Shader;

EnemySpawner::EnemySpawner(Spawn spawn, ParticleEmitter explode) : m_spawn(std::move(spawn)), m_explode(std::move(explode)) {
	m_explode.config.respawn = false;
}

void EnemySpawner::tick(Seconds const dt) {
	for (auto const& enemy : m_enemies) {
		enemy->tick(dt);
		if (enemy->is_destroyed()) { explode_at(enemy->get_bounds().centre()); }
	}

	for (auto& emitter : m_explodes) { emitter.tick(dt); }

	std::erase_if(m_enemies, [](auto const& enemy) { return enemy->is_destroyed(); });
	std::erase_if(m_explodes, [](ParticleEmitter const& emitter) { return emitter.active_particles() == 0; });
}

void EnemySpawner::draw(Shader& shader) const {
	for (auto const& enemy : m_enemies) { enemy->draw(shader); }
	for (auto const& emitter : m_explodes) { emitter.draw(shader); }
}

void EnemySpawner::append_targets(std::vector<bave::NotNull<IDamageable*>>& out) const {
	out.reserve(out.size() + m_enemies.size());
	for (auto const& enemy : m_enemies) { out.push_back(enemy.get()); }
}

void EnemySpawner::explode_at(glm::vec2 const position) {
	auto& emitter = m_explodes.emplace_back(m_explode);
	emitter.set_position(position);
}
} // namespace spaced
