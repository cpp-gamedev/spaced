#pragma once
#include <spaced/game/enemy_factory.hpp>
#include <spaced/game/player.hpp>
#include <spaced/game/scorer.hpp>
#include <spaced/game/target_provider.hpp>
#include <spaced/game/tiled_bg.hpp>

namespace bave {
struct Resources;
}

namespace spaced {
struct Stats;

class World : public ITargetProvider {
  public:
	explicit World(bave::NotNull<bave::Services const*> services, bave::NotNull<IScorer*> scorer);

	[[nodiscard]] auto get_targets() const -> std::span<bave::NotNull<IDamageable*> const> final { return m_targets; }

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const;

	void on_death(Enemy const& enemy, bool add_score);

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	Player player;

  private:
	void do_inspect();
	void inspect_enemies();

	void debug_controller_type();
	void debug_spawn_powerup(glm::vec2 position);

	bave::NotNull<bave::Services const*> m_services;
	bave::NotNull<bave::Resources const*> m_resources;
	bave::NotNull<bave::IAudio*> m_audio;
	bave::NotNull<Stats*> m_stats;
	bave::NotNull<IScorer*> m_scorer;

	TiledBg m_background;

	std::unordered_map<std::string, std::unique_ptr<EnemyFactory>> m_enemy_factories{};

	std::vector<std::unique_ptr<Enemy>> m_active_enemies{};
	std::vector<bave::ParticleEmitter> m_enemy_death_emitters{};
	std::vector<std::unique_ptr<IPowerup>> m_active_powerups{};

	std::vector<bave::NotNull<IDamageable*>> m_targets{};
	std::vector<bave::NotNull<IPowerup*>> m_powerups{};
};
} // namespace spaced
