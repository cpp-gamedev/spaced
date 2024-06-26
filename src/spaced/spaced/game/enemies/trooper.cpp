#include <bave/services/resources.hpp>
#include <spaced/game/enemies/trooper.hpp>

namespace spaced::enemy {
using bave::NotNull;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Texture;

Trooper::Trooper(Services const& services, NotNull<GunKinetic*> gun) : Enemy(services, "Trooper"), m_gun(gun) {
	if (auto texture = services.get<Resources>().get<Texture>("images/ship_trooper.png")) {
		texture->sampler.wrap_s = texture->sampler.wrap_t = Texture::Wrap::eClampEdge;
		m_sprite.set_size(texture->get_size());
		m_sprite.set_texture(std::move(texture));
	}

	health = 3.0f;
	speed = 120.0f;
}

void Trooper::do_tick(Seconds const dt) { m_fire_elapsed += dt; }

auto Trooper::make_round() -> std::unique_ptr<IWeaponRound> {
	if (m_fire_elapsed < m_fire_rate) { return {}; }

	m_fire_elapsed = 0s;
	return m_gun->fire(get_muzzle_position());
}

auto Trooper::get_muzzle_position() const -> glm::vec2 {
	auto ret = m_sprite.transform.position;
	ret.x -= m_sprite.get_size().x;
	return ret;
}
} // namespace spaced::enemy
