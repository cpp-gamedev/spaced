#include <spaced/game/enemies/creep.hpp>

namespace spaced {
using bave::Seconds;

void Creep::tick(Seconds const dt) {
	sprite.tick(dt);

	sprite.transform.position.x -= x_speed * dt.count();
	if (sprite.transform.position.x < -0.5f * (get_layout().get_world_space().x + sprite.get_size().x)) { health = 0.0f; }
}
} // namespace spaced
