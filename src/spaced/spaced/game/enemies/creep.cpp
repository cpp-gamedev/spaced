#include <bave/services/resources.hpp>
#include <spaced/game/enemies/creep.hpp>

namespace spaced {
using bave::Resources;
using bave::Seconds;
using bave::Texture;

Creep::Creep(bave::Services const& services) : Enemy(services, "Creep") {
	sprite.set_texture(services.get<Resources>().get<Texture>("images/creep_ship.png"));
	sprite.set_size(glm::vec2{80.0f});
	health = 2.0f;
}

void Creep::tick(Seconds const dt, bool const in_play) {
	Enemy::tick(dt, in_play);
	if (!in_play) { return; }

	sprite.transform.position.x -= x_speed * dt.count();
	if (sprite.transform.position.x < -0.5f * (get_layout().world_space.x + sprite.get_shape().size.x)) { set_destroyed(); }
}
} // namespace spaced
