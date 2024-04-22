#include <spaced/game/enemy_factory.hpp>
#include <spaced/services/audio.hpp>

namespace spaced {
using bave::NotNull;
using bave::Seconds;

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
