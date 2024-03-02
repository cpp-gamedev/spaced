#pragma once
#include <bave/core/time.hpp>
#include <glm/vec2.hpp>

namespace spaced {
class Damage {
  public:
	void inflict(float amount);
	bool is_dead() const;

  private:
	void update();

	int health{};
	bool dead{};
};
} // namespace spaced
