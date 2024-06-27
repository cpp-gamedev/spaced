#include <bave/core/random.hpp>
#include <bave/services/resources.hpp>
#include <spaced/game/enemies/trooper.hpp>

namespace spaced::enemy {
using bave::NotNull;
using bave::random_in_range;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Texture;

Trooper::Trooper(Services const& services, NotNull<GunKinetic*> gun) : GunnerBase(services, gun, "Trooper") {
	if (auto texture = services.get<Resources>().get<Texture>("images/ship_trooper.png")) {
		m_sprite.set_size(texture->get_size());
		m_sprite.set_texture(std::move(texture));
	}

	m_direction = random_in_range(0, 1) == 0 ? 1.0f : -1.0f;

	health = 3.0f;
	speed = 150.0f;
}

void Trooper::do_tick(Seconds const dt) {
	GunnerBase::do_tick(dt);

	m_sprite.transform.position.y += m_direction * m_y_speed * dt.count();
	auto const y_min = get_layout().play_area.rb.y + m_sprite.get_size().y;
	auto const y_max = get_layout().play_area.lt.y - m_sprite.get_size().y;
	if (m_sprite.transform.position.y < y_min) {
		m_direction = 1.0f;
	} else if (m_sprite.transform.position.y > y_max) {
		m_direction = -1.0f;
	}
}
} // namespace spaced::enemy
