#include <bave/core/fixed_string.hpp>
#include <bave/imgui/im_text.hpp>
#include <spaced/game/enemy_spawner.hpp>
#include <spaced/services/resources.hpp>

namespace spaced {
using bave::FixedString;
using bave::im_text;
using bave::ParticleEmitter;
using bave::Seconds;
using bave::Shader;

EnemySpawner::EnemySpawner(std::unique_ptr<IEnemyFactory> factory) : m_factory(std::move(factory)) {
	if (!m_factory) { throw std::runtime_error{"Null EnemyFactory passed to EnemySpawner"}; }
}

void EnemySpawner::tick(Seconds const dt, bool const in_play) {
	if (m_factory->tick(dt)) { spawn(); }

	for (auto const& enemy : m_enemies) {
		enemy->tick(dt, in_play);
		if (enemy->is_dead()) { explode_at(enemy->get_bounds().centre()); }
	}

	for (auto& emitter : m_death_emitters) { emitter.tick(dt); }

	std::erase_if(m_enemies, [](auto const& enemy) { return enemy->is_destroyed(); });
	std::erase_if(m_death_emitters, [](ParticleEmitter const& emitter) { return emitter.active_particles() == 0; });
}

void EnemySpawner::draw(Shader& shader) const {
	for (auto const& enemy : m_enemies) { enemy->draw(shader); }
	for (auto const& emitter : m_death_emitters) { emitter.draw(shader); }
}

void EnemySpawner::append_targets(std::vector<bave::NotNull<IDamageable*>>& out) const {
	out.reserve(out.size() + m_enemies.size());
	for (auto const& enemy : m_enemies) { out.emplace_back(enemy.get()); }
}

void EnemySpawner::explode_at(glm::vec2 const position) {
	auto& emitter = m_death_emitters.emplace_back(m_factory->get_death_emitter());
	emitter.config.respawn = false;
	emitter.set_position(position);
	m_factory->play_death_sfx();
}

void EnemySpawner::do_inspect() {
	if constexpr (bave::imgui_v) {
		im_text("{}", m_factory->get_type_name());
		if (ImGui::Button("spawn")) { spawn(); }

		ImGui::Separator();
		m_factory->inspect();

		for (std::size_t i = 0; i < m_enemies.size(); ++i) {
			if (ImGui::TreeNode(FixedString{"{}", i}.c_str())) {
				m_enemies.at(i)->inspect();
				ImGui::TreePop();
			}
		}
	}
}
} // namespace spaced
