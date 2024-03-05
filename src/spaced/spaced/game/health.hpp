#pragma once
#include <bave/platform.hpp>

namespace spaced {
class Health {
  public:
	static constexpr auto hit_points_v{1.0f};

	/*implicit*/ Health(float const hit_points = hit_points_v) : m_hit_points(hit_points) {}

	void inflict_damage(float amount);
	[[nodiscard]] bool is_dead() const;
	[[nodiscard]] float get_hit_points() const;
	void set_hit_points(float hit_points);

	operator float() const { return get_hit_points(); }

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

  private:
	void do_inspect();

	float m_hit_points{};
};
} // namespace spaced
