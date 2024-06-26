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

	void debug_spawn_powerup(glm::vec2 position);

	SignalHandle m_on_player_scored{};
	GunKinetic m_trooper_gun;

	std::vector<std::unique_ptr<SpawnTimer<Enemy>>> m_spawn_timers{};
};
} // namespace spaced
