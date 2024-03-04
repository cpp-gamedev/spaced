#pragma once
#include <algorithm>

namespace spaced {
class Health {
  public:
	void inflict_damage(float amount);
	bool is_dead() const;
	int get_health() const;

	void debug_stuff();

  private:
	float m_health{25};
};
} // namespace spaced
