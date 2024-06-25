#include <bave/core/random.hpp>
#include <spaced/game/powerups/pu_beam.hpp>
#include <spaced/scenes/endless.hpp>
#include <spaced/services/game_signals.hpp>

namespace spaced {
using bave::App;
using bave::random_in_range;
using bave::Seconds;
using bave::Services;

EndlessScene::EndlessScene(App& app, Services const& services) : GameScene(app, services), m_creeps(&services) {
	m_on_player_scored = services.get<GameSignals>().player_scored.connect([this](Enemy const& e) {
		if (random_in_range(0, 10) < 3) { debug_spawn_powerup(e.get_position()); }
	});
}

void EndlessScene::tick(Seconds const dt) {
	GameScene::tick(dt);

	if (is_game_over()) { return; }

	if (auto creep = m_creeps.tick(dt)) { push_enemy(std::move(creep)); }
}

void EndlessScene::debug_spawn_powerup(glm::vec2 const position) {
	auto powerup = std::make_unique<PUBeam>(get_services());
	powerup->shape.transform.position = position;
	push_powerup(std::move(powerup));
}
} // namespace spaced
