#include <bave/core/random.hpp>
#include <spaced/game/enemies/creep.hpp>
#include <spaced/game/powerups/beam.hpp>
#include <spaced/scenes/endless.hpp>
#include <spaced/services/game_signals.hpp>

namespace spaced {
using bave::App;
using bave::random_in_range;
using bave::Seconds;
using bave::Services;

EndlessScene::EndlessScene(App& app, Services const& services) : GameScene(app, services) {
	m_on_player_scored = services.get<GameSignals>().player_scored.connect([this](Enemy const& e) {
		if (random_in_range(0, 10) < 3) { debug_spawn_powerup(e.get_position()); }
	});

	m_spawn_timers.push_back(std::make_unique<SpawnTimer<enemy::Creep>>(&services));
}

void EndlessScene::tick(Seconds const dt) {
	GameScene::tick(dt);

	if (is_game_over()) { return; }

	for (auto const& spawn_timer : m_spawn_timers) {
		if (auto enemy = spawn_timer->tick(dt)) { push_enemy(std::move(enemy)); }
	}
}

void EndlessScene::debug_spawn_powerup(glm::vec2 const position) {
	auto powerup = std::make_unique<powerup::Beam>(get_services());
	powerup->shape.transform.position = position;
	push_powerup(std::move(powerup));
}
} // namespace spaced
