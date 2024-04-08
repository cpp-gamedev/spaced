#include <spaced/game/motion.hpp>

namespace spaced {

void Motion::tick(bave::Seconds dt) { m_velocity += m_acceleration * dt.count(); }

void Motion::apply_force(glm::vec2 force) { m_acceleration = force; }

void Motion::dampen(float factor) { m_velocity *= factor; }

} // namespace spaced
