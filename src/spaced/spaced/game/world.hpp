#pragma once
#include <spaced/game/enemy_factory.hpp>
#include <spaced/game/player.hpp>
#include <spaced/game/scorer.hpp>
#include <spaced/game/target_provider.hpp>
#include <spaced/game/world_spec.hpp>

namespace spaced {
class Resources;

class World : public ITargetProvider, public IEnemyDeathListener {
  public:
	explicit World(bave::NotNull<Services const*> services, bave::NotNull<IScorer*> scorer);

	[[nodiscard]] auto get_targets() const -> std::span<bave::NotNull<IDamageable*> const> final { return m_targets; }

	void on_death(EnemyDeath const& death) final;

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const;

	void on_death(Enemy2 const& enemy, bool add_score);

	void load(WorldSpec const& spec);

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	Player player;

  private:
	void do_inspect();
	void inspect_enemies();

	void debug_controller_type();
	void debug_spawn_powerup(glm::vec2 position);

	bave::NotNull<Services const*> m_services;
	bave::NotNull<Resources const*> m_resources;
	bave::NotNull<IAudio*> m_audio;
	bave::NotNull<IScorer*> m_scorer;

	std::unordered_map<std::string, std::unique_ptr<EnemyFactory>> m_enemy_factories{};

	std::vector<std::unique_ptr<Enemy2>> m_active_enemies{};
	std::vector<bave::ParticleEmitter> m_enemy_death_emitters{};
	std::vector<std::unique_ptr<IPowerup>> m_active_powerups{};

	std::vector<bave::NotNull<IDamageable*>> m_targets{};
	std::vector<bave::NotNull<IPowerup*>> m_powerups{};
};
} // namespace spaced
