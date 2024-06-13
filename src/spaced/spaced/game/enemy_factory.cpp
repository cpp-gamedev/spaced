#include <bave/services/audio.hpp>
#include <spaced/game/enemy_factory.hpp>

namespace spaced {
using bave::IAudio;
using bave::NotNull;
using bave::Seconds;
using bave::Services;

EnemyFactory::EnemyFactory(NotNull<Services const*> services) : m_services(services), m_audio(&services->get<IAudio>()) {}

auto EnemyFactory::tick(Seconds const dt) -> std::unique_ptr<Enemy> {
	if (spawn_rate <= 0s) { return {}; }

	m_elapsed += dt;
	if (m_elapsed >= spawn_rate) {
		m_elapsed = 0s;
		return spawn_enemy();
	}
	return {};
}
} // namespace spaced
