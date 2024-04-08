#include <glm/gtx/norm.hpp>
#include <spaced/game/Gravitator.hpp>

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
	auto const acceleration = glm::vec2{force_x, force_y};

	auto const dv = acceleration * time_slice.count();
	m_velocity += dv;
	m_velocity *= (1.0f - dampen);
	position += m_velocity * time_slice.count();
}
} // namespace spaced
