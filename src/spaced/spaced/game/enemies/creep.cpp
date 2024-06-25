#include <bave/services/resources.hpp>
#include <spaced/game/enemies/creep.hpp>

namespace spaced {
using bave::Resources;
using bave::Seconds;
using bave::Texture;

Creep::Creep(bave::Services const& services) : Enemy(services, "Creep") {
	m_sprite.set_texture(services.get<Resources>().get<Texture>("images/creep_ship.png"));
	m_sprite.set_size(glm::vec2{80.0f});
	health = 2.0f;
}

void Creep::tick(Seconds const dt, bool const in_play) {
	if (!in_play) { return; }

	m_sprite.transform.position.x -= x_speed * dt.count();
	if (m_sprite.transform.position.x < -0.5f * (get_layout().world_space.x + m_sprite.get_shape().size.x)) { set_destroyed(); }

	update_health_bar();
}
} // namespace spaced
