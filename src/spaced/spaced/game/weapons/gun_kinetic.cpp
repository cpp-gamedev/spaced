#include <spaced/game/weapons/gun_kinetic.hpp>

namespace spaced {
using bave::Seconds;

GunKinetic::GunKinetic(Services const& services) : Weapon(services, "GunKinetic") {}

auto GunKinetic::fire(glm::vec2 const muzzle_position) -> std::unique_ptr<Round> {
	if (m_reload_remain > 0s) { return {}; }

	m_reload_remain = reload_delay;
	return std::make_unique<Projectile>(&get_layout(), projectile_config, muzzle_position);
}

void GunKinetic::tick(Seconds const dt) {
	if (m_reload_remain > 0s) { m_reload_remain -= dt; }
}
} // namespace spaced
