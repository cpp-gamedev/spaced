#pragma once
#include <bave/core/polymorphic.hpp>
#include <bave/core/time.hpp>
#include <bave/input/event.hpp>
#include <bave/platform.hpp>

namespace spaced {
class IController : public bave::Polymorphic {
  public:
	[[nodiscard]] virtual auto get_type_name() const -> std::string_view = 0;

	virtual void on_move(bave::PointerMove const& pointer_move) = 0;
	virtual void on_tap(bave::PointerTap const& pointer_tap) = 0;

	virtual void untap() = 0;

	virtual auto tick(bave::Seconds dt) -> float = 0;

	[[nodiscard]] virtual auto is_firing() const -> bool = 0;
	virtual void stop_firing() = 0;

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

  protected:
	virtual void do_inspect() {}
};
} // namespace spaced
