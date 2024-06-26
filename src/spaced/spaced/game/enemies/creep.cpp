#include <bave/services/resources.hpp>
#include <spaced/game/enemies/creep.hpp>

namespace spaced::enemy {
using bave::Resources;
using bave::Seconds;
using bave::Texture;

Creep::Creep(bave::Services const& services) : Enemy(services, "Creep") {
	m_sprite.set_texture(services.get<Resources>().get<Texture>("images/creep_ship.png"));
	m_sprite.set_size(glm::vec2{80.0f});
	health = 2.0f;
}

auto Creep::tick(Seconds const dt) -> std::unique_ptr<IWeaponRound> {
	m_sprite.transform.position.x -= m_speed * dt.count();
	if (m_sprite.transform.position.x < -0.5f * (get_layout().world_space.x + m_sprite.get_shape().size.x)) { set_destroyed(); }

	update_health_bar();

	return {};
}
} // namespace spaced::enemy
