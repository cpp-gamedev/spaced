#pragma once
#include <spaced/game/spawn_timer.hpp>
#include <spaced/game/weapons/gun_kinetic.hpp>
#include <spaced/scenes/game.hpp>
#include <spaced/signal.hpp>

namespace spaced {
class EndlessScene : public GameScene {
  public:
	EndlessScene(bave::App& app, bave::Services const& services);

  private:
	void on_loaded() final;
	void tick(bave::Seconds dt) final;

	void on_start() final;

	void ramp_difficulty();
	void on_player_scored(Enemy const& enemy);

	[[nodiscard]] auto make_beam_powerup() const -> std::unique_ptr<Powerup>;
	[[nodiscard]] auto make_1up_powerup() const -> std::unique_ptr<Powerup>;

	[[nodiscard]] auto make_creep_spawner() const -> std::unique_ptr<SpawnTimer<Enemy>>;
	[[nodiscard]] auto make_gunner_spawner() -> std::unique_ptr<SpawnTimer<Enemy>>;
	[[nodiscard]] auto make_trooper_spawner() -> std::unique_ptr<SpawnTimer<Enemy>>;

	void do_inspect(bave::Seconds dt) final;

	SignalHandle m_on_player_scored{};
	GunKinetic m_enemy_gun;

	bave::Seconds m_elapsed{};
	bave::Seconds m_since_powerup{};
	std::vector<std::unique_ptr<SpawnTimer<Enemy>>> m_spawn_timers{};

	struct {
		bave::Ptr<SpawnTimer<Enemy>> creep{};
		bave::Ptr<SpawnTimer<Enemy>> gunner{};
		bave::Ptr<SpawnTimer<Enemy>> trooper{};
	} m_spawner{};
};
} // namespace spaced
