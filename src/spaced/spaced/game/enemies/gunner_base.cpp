#include <spaced/game/enemies/gunner_base.hpp>

namespace spaced::enemy {
using bave::NotNull;
using bave::Seconds;
using bave::Services;

GunnerBase::GunnerBase(Services const& services, NotNull<GunKinetic*> gun, std::string_view const type) : Enemy(services, type), m_gun(gun) {}

void GunnerBase::do_tick(Seconds const dt) { m_fire_elapsed += dt; }

auto GunnerBase::make_round() -> std::unique_ptr<IWeaponRound> {
	if (m_fire_elapsed < m_fire_rate) { return {}; }

	m_fire_elapsed = 0s;
	return m_gun->fire(get_muzzle_position());
}

auto GunnerBase::get_muzzle_position() const -> glm::vec2 {
	auto ret = m_sprite.transform.position;
	ret.x -= m_sprite.get_size().x;
	return ret;
}
} // namespace spaced::enemy
