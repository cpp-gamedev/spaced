#include <bave/imgui/im_text.hpp>
#include <bave/services/audio.hpp>
#include <spaced/game/weapon.hpp>

namespace spaced {
using bave::IAudio;
using bave::IDisplay;
using bave::im_text;
using bave::Services;

Weapon::Weapon(Services const& services, std::string name) : m_log{std::move(name)}, m_display(&services.get<IDisplay>()), m_audio(&services.get<IAudio>()) {}

auto Weapon::fire(glm::vec2 const muzzle_position) -> std::unique_ptr<Round> {
	auto ret = do_fire(muzzle_position);
	if (ret) { m_audio->play_any_sfx(m_fire_sfx); }
	return ret;
}

void Weapon::do_inspect() {
	if constexpr (bave::imgui_v) { im_text("rounds remaining: {}", get_rounds_remaining()); }
}
} // namespace spaced
