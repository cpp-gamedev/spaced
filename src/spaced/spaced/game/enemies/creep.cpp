#include <bave/services/resources.hpp>
#include <spaced/game/enemies/creep.hpp>

namespace spaced::enemy {
using bave::Resources;
using bave::Texture;

Creep::Creep(bave::Services const& services) : Enemy(services, "Creep") {
	if (auto texture = services.get<Resources>().get<Texture>("assets/images/ship_creep.png")) {
		m_sprite.set_size(texture->get_size());
		m_sprite.set_texture(std::move(texture));
	}

	health = 1.0f;
	speed = 100.0f;
	points = 10;
}
} // namespace spaced::enemy
