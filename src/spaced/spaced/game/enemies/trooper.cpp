#include <bave/services/resources.hpp>
#include <spaced/game/enemies/trooper.hpp>

namespace spaced::enemy {
using bave::NotNull;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Texture;

Trooper::Trooper(Services const& services, NotNull<GunKinetic*> gun) : Enemy(services, "Creep"), m_gun(gun) {
	auto const& resources = services.get<Resources>();

	if (auto texture = resources.get<Texture>("images/ship_trooper.png")) {
		texture->sampler.wrap_s = texture->sampler.wrap_t = Texture::Wrap::eClampEdge;
		m_sprite.set_size(texture->get_size());
		m_sprite.set_texture(std::move(texture));
	}
	health = 2.0f;
}

auto Trooper::tick(Seconds const dt) -> std::unique_ptr<IWeaponRound> {
	m_sprite.transform.position.x -= m_speed.x * dt.count();
	if (m_sprite.transform.position.x < -0.5f * (get_layout().world_space.x + m_sprite.get_shape().size.x)) { set_destroyed(); }

	auto ret = std::unique_ptr<IWeaponRound>{};
	m_fire_elapsed += dt;
	if (m_fire_elapsed >= m_fire_rate) {
		m_fire_elapsed = 0s;
		ret = m_gun->fire(get_muzzle_position());
	}

	update_health_bar();

	return ret;
}

auto Trooper::get_muzzle_position() const -> glm::vec2 {
	auto ret = m_sprite.transform.position;
	ret.x -= m_sprite.get_size().x;
	return ret;
}
} // namespace spaced::enemy
