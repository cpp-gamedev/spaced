#include <spaced/scenes/endless.hpp>

namespace spaced {
using bave::App;
using bave::Seconds;
using bave::Services;

EndlessScene::EndlessScene(App& app, Services const& services) : GameScene(app, services), m_creeps(&services) {}

void EndlessScene::tick(Seconds const dt) {
	GameScene::tick(dt);

	if (is_game_over()) { return; }

	if (auto creep = m_creeps.tick(dt)) { push_enemy(std::move(creep)); }
}
} // namespace spaced
