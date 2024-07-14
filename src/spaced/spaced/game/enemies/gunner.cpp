#include <bave/services/resources.hpp>
#include <spaced/game/enemies/gunner.hpp>

namespace spaced::enemy {
using bave::Resources;
using bave::Texture;

Gunner::Gunner(bave::Services const& services, bave::NotNull<GunKinetic*> gun) : GunnerBase(services, gun, "Gunner") {
	if (auto texture = services.get<Resources>().get<Texture>("assets/images/ship_gunner.png")) {
		m_sprite.set_size(texture->get_size());
		m_sprite.set_texture(std::move(texture));
	}

	health = 3.0f;
	speed = 120.0f;
	points = 20;
}
} // namespace spaced::enemy
