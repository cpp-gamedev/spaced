#include <bave/services/resources.hpp>
#include <spaced/game/enemies/creep.hpp>
#include <spaced/game/enemies/creep_factory.hpp>

namespace spaced {
using bave::NotNull;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Texture;

CreepFactory::CreepFactory(NotNull<Services const*> services)
	: EnemyFactory(services), m_ship_texture(services->get<Resources>().get<Texture>("images/creep_ship.png")) {}

auto CreepFactory::spawn_enemy() -> std::unique_ptr<Enemy> {
	auto ret = std::make_unique<Creep>(get_services());
	ret->sprite.set_texture(m_ship_texture);
	ret->sprite.set_size(glm::vec2{80.0f});
	ret->health = m_initial_health;
	return ret;
}
} // namespace spaced
