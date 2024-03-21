#pragma once
#include <spaced/game/enemy_spawner.hpp>
#include <spaced/game/player.hpp>
#include <spaced/game/scorer.hpp>
#include <spaced/game/target_provider.hpp>
#include <spaced/game/world_spec.hpp>

namespace spaced {
class World : public ITargetProvider, public IEnemyDeathListener {
  public:
	explicit World(bave::NotNull<Services const*> services, bave::NotNull<IScorer*> scorer);

	[[nodiscard]] auto get_targets() const -> std::span<bave::NotNull<IDamageable*> const> final { return m_targets; }

	void on_death(EnemyDeath const& death) final;

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const;

	void load(WorldSpec const& spec);

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	Player player;

  private:
	void do_inspect();
	void inspect_enemy_spawners();

	void debug_controller_type();
	void debug_spawn_powerup(glm::vec2 position);

	bave::NotNull<Services const*> m_services;
	bave::NotNull<IScorer*> m_scorer;

	std::vector<EnemySpawner> m_enemy_spawners{};
	std::vector<std::unique_ptr<IPowerup>> m_powerups{};

	std::vector<bave::NotNull<IDamageable*>> m_targets{};
	std::vector<bave::NotNull<IPowerup*>> m_powerups_view{};
};
} // namespace spaced
