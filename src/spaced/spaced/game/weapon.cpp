#include <bave/imgui/im_text.hpp>
#include <spaced/game/weapon.hpp>

namespace spaced {
using bave::IAudio;
using bave::im_text;
using bave::Services;

Weapon::Weapon(Services const& services, std::string name) : m_log{std::move(name)}, m_layout(&services.get<Layout>()), m_audio(&services.get<IAudio>()) {}

auto Weapon::fire(glm::vec2 const muzzle_position) -> std::unique_ptr<Round> { return do_fire(muzzle_position); }

void Weapon::do_inspect() {
	if constexpr (bave::imgui_v) { im_text("rounds remaining: {}", get_rounds_remaining()); }
}
} // namespace spaced
