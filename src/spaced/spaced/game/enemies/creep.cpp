#include <bave/services/resources.hpp>
#include <spaced/game/enemies/creep.hpp>

namespace spaced::enemy {
using bave::Resources;
using bave::Seconds;
using bave::Texture;

Creep::Creep(bave::Services const& services) : Enemy(services, "Creep") {
	if (auto texture = services.get<Resources>().get<Texture>("images/ship_creep.png")) {
		m_sprite.set_size(texture->get_size());
		m_sprite.set_texture(std::move(texture));
	}
	health = 2.0f;
}

auto Creep::tick(Seconds const dt) -> std::unique_ptr<IWeaponRound> {
	m_sprite.transform.position.x -= m_speed * dt.count();
	if (m_sprite.transform.position.x < -0.5f * (get_layout().world_space.x + m_sprite.get_shape().size.x)) { set_destroyed(); }

	update_health_bar();

	return {};
}
} // namespace spaced::enemy
