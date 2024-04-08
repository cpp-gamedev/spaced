#include <glm/gtx/norm.hpp>
#include <spaced/game/gravitator.hpp>

namespace spaced {
void Gravitator::tick(bave::Seconds dt) {
	dt += m_residue;
	if (dt > max_dt) { return; }
	for (; dt > 0s; dt -= time_slice) { integrate(); }
	m_residue = dt;
}

void Gravitator::integrate() {
	auto const to_target = target - position;

	if (glm::length2(to_target) < min_distance * min_distance) {
		position = target;
		return;
	}

	auto const gx = position.x;
	auto const gy = position.y;
	auto const mx = target.x;
	auto const my = target.y;

	auto force_x = mx - gx;
	auto force_y = my - gy;
	auto mag = sqrt((force_x * force_x) + (force_y * force_y));
	mag = std::max(tiny_value, mag);
	auto strength = force / mag * mag;
	force_x *= strength;
	force_y *= strength;
	motion.apply_force({force_x, force_y});
	motion.dampen(1.0f - dampen);
	position += motion.velocity() * time_slice.count();
}
} // namespace spaced
