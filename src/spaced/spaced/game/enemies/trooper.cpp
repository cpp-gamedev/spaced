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

namespace {
constexpr auto at_end(float const y, float const y_min, float const y_max) { return y_min > y || y > y_max; }
} // namespace

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
	if (at_end(m_sprite.transform.position.y, y_min, y_max)) { m_direction = -m_direction; }
}
} // namespace spaced::enemy
