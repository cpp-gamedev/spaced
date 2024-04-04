#include <spaced/game/enemies/creep.hpp>

namespace spaced {
using bave::Seconds;

void Creep::tick(Seconds const dt, bool const in_play) {
	Enemy::tick(dt, in_play);
	if (!in_play) { return; }

	shape.transform.position.x -= x_speed * dt.count();
	if (shape.transform.position.x < -0.5f * (get_layout().get_world_space().x + shape.get_shape().size.x)) { set_destroyed(); }
}
} // namespace spaced
