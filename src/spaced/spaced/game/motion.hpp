#pragma once
#include <bave/core/time.hpp>
#include <glm/vec2.hpp>

namespace spaced {
class Motion {
  public:
	[[nodiscard]] auto acceleration() const -> glm::vec2 { return m_acceleration; }
	[[nodiscard]] auto velocity() const -> glm::vec2 { return m_velocity; }

	void tick(bave::Seconds dt);
	void apply_force(glm::vec2 force);
	void dampen(float factor);

  private:
	glm::vec2 m_acceleration{};
	glm::vec2 m_velocity{};
};
} // namespace spaced
