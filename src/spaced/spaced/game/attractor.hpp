#pragma once
#include <bave/core/time.hpp>
#include <glm/vec2.hpp>

namespace spaced {
class Attractor {
  public:
	glm::vec2 target{};
	glm::vec2 position{};

	bave::Seconds time_slice{1.0f / 250.0f};
	bave::Seconds max_dt{0.8s};
	float dampen{0.9f};
	float force{0.01f};
	float min_distance{0.1f};

	void tick(bave::Seconds dt);

  private:
	void integrate();

	glm::vec2 m_velocity{};
	bave::Seconds m_residue{};
};
} // namespace spaced
