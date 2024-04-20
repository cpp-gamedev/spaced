#include <spaced/game/enemy_factory.hpp>
#include <spaced/services/audio.hpp>

namespace spaced {
using bave::NotNull;

IEnemyFactory::IEnemyFactory(NotNull<Services const*> services) : m_services(services), m_audio(&services->get<IAudio>()) {}

void IEnemyFactory::play_death_sfx() { m_audio->play_any_sfx(m_death_sfx); }
} // namespace spaced
