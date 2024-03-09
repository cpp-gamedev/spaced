#include <glm/gtx/norm.hpp>
#include <spaced/game/Attractor.hpp>

namespace spaced {

using bave::RoundedQuad;

Attractor::Attractor(Services const& services) {
	auto rounded_quad = RoundedQuad{};
	rounded_quad.size = glm::vec2{20.0f};
	rounded_quad.corner_radius = 5.0f;
	shape.set_shape(rounded_quad);
}
void Attractor::tick(bave::Seconds dt) {
	dt += m_residue;
	if (dt > max_dt) { return; }
	for (; dt > 0s; dt -= time_slice) { integrate(); }
	m_residue = dt;
}

void Attractor::integrate() {

	auto const gx = position.x;
	auto const gy = position.y;
	auto const mx = target.x;
	auto const my = target.y;

	auto force_x = mx - gx;
	auto force_y = my - gy;
	auto const mag = sqrt((force_x * force_x) + (force_y * force_y));
	auto strength = force / mag * mag;
	force_x *= strength;
	force_y *= strength;
	auto const acceleration = glm::vec2{force_x, force_y};

	auto const dv = acceleration * time_slice.count();
	m_velocity += dv;
	m_velocity *= (1.0f - dampen);
	position += m_velocity * time_slice.count();
	shape.transform.position = position;
}
} // namespace spaced