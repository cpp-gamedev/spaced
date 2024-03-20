#include <bave/imgui/im_text.hpp>
#include <spaced/game/weapon.hpp>

namespace spaced {
using bave::im_text;

void Weapon::do_inspect() {
	if constexpr (bave::imgui_v) { im_text("rounds remaining: {}", get_rounds_remaining()); }
}
} // namespace spaced
