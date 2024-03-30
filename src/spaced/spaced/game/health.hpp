#pragma once
#include <bave/platform.hpp>

namespace spaced {
class Health {
  public:
	static constexpr auto hit_points_v{1.0f};

	/*implicit*/ Health(float const hit_points = hit_points_v) : m_total_hp(hit_points), m_current_hp(hit_points) {}

	void inflict_damage(float amount);
	[[nodiscard]] bool is_dead() const;
	[[nodiscard]] float get_hit_points() const;
	[[nodiscard]] float get_total_hit_points() const { return m_total_hp; }
	void set_hit_points(float hit_points);

	operator float() const { return get_hit_points(); }

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

  private:
	void do_inspect();

	float m_total_hp{};
	float m_current_hp{};
};
} // namespace spaced
