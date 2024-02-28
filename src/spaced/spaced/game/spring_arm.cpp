#include <glm/gtx/norm.hpp>
#include <spaced/game/spring_arm.hpp>

namespace spaced {
void SpringArm::tick(bave::Seconds dt) {
	dt += m_residue;
	if (dt > max_dt) { return; }
	for (; dt > 0s; dt -= time_slice) { integrate(); }
	m_residue = dt;
}

void SpringArm::integrate() {
	auto const to_target = target - position;

	if (glm::length2(to_target) < min_distance * min_distance) {
		position = target;
		return;
	}

	auto const acceleration = k * to_target;
	auto const dv = acceleration * time_slice.count();
	m_velocity += dv;
	m_velocity *= (1.0f - damp);
	position += m_velocity * time_slice.count();
}
} // namespace spaced
