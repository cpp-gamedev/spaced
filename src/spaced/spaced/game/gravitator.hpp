#pragma once
#include <bave/core/time.hpp>
#include <glm/vec2.hpp>
#include <spaced/game/motion.hpp>

namespace spaced {
float const tiny_value{0.001f};
class Gravitator {
  public:
	glm::vec2 target{};
	glm::vec2 position{};

	bave::Seconds time_slice{1.0f / 250.0f};
	bave::Seconds max_dt{0.8s};
	float dampen{0.9f};
	float force{0.01f};
	float min_distance{0.1f};

	void tick(bave::Seconds dt);

	Motion motion{};

  private:
	void integrate();

	bave::Seconds m_residue{};
};
} // namespace spaced
