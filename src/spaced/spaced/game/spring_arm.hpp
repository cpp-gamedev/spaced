#pragma once
#include <bave/core/time.hpp>
#include <glm/vec2.hpp>

namespace spaced {
class SpringArm {
  public:
	glm::vec2 target{};
	float k{200.0f};
	float damp{0.05f};

	glm::vec2 position{};

	bave::Seconds time_slice{1.0f / 250.0f};
	bave::Seconds max_dt{0.8s};
	float min_distance{0.01f};

	void tick(bave::Seconds dt);

  private:
	void integrate();

	glm::vec2 m_velocity{};
	bave::Seconds m_residue{};
};
} // namespace spaced
