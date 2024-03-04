#pragma once

namespace spaced {
class Health {
  public:
	void inflict_damage(float amount);
	[[nodiscard]] bool is_dead() const;
	[[nodiscard]] float get_health() const;

	void debug_stuff();

  private:
	float m_health{25};
};
} // namespace spaced
