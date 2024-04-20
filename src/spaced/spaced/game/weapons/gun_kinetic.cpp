#include <bave/imgui/im_text.hpp>
#include <spaced/game/weapons/gun_kinetic.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::im_text;
using bave::Rgba;
using bave::Seconds;

GunKinetic::GunKinetic(Services const& services) : Weapon(services, "GunKinetic"), m_audio(&services.get<IAudio>()) {
	projectile_config.tint = services.get<Styles>().rgbas["black"];
}

void GunKinetic::tick(Seconds const dt) {
	if (m_reload_remain > 0s) { m_reload_remain -= dt; }
}

auto GunKinetic::do_fire(glm::vec2 const muzzle_position) -> std::unique_ptr<Round> {
	if (m_reload_remain > 0s || rounds == 0) { return {}; }

	if (rounds > 0) { --rounds; }
	m_reload_remain = reload_delay;
	return std::make_unique<Projectile>(&get_layout(), projectile_config, muzzle_position);
}

void GunKinetic::do_inspect() {
	if constexpr (bave::imgui_v) {
		im_text("type: GunKinetic");
		Weapon::do_inspect();
		ImGui::DragFloat2("projectile size", &projectile_config.size.x);
		ImGui::DragFloat("x speed", &projectile_config.x_speed, 10.0f, 100.0f, 10000.0f);
		auto tint = projectile_config.tint.to_vec4();
		if (ImGui::ColorEdit4("projectile tint", &tint.x)) { projectile_config.tint = Rgba::from(tint); }
		ImGui::DragFloat("damage", &projectile_config.damage, 0.25f, 0.25f, 10.0f);
	}
}
} // namespace spaced
