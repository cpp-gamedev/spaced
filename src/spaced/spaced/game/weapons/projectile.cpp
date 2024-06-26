#include <spaced/game/weapons/projectile.hpp>

namespace spaced {
using bave::NotNull;
using bave::Rect;
using bave::Seconds;
using bave::Shader;

namespace {
[[nodiscard]] constexpr auto check_hit(glm::vec2 const position, glm::vec2 const size, float const dx, Rect<> const& target) -> bool {
	auto rect = Rect<>::from_size(size, position);
	rect.lt.x -= dx;
	return is_intersecting(rect, target);
}

[[nodiscard]] constexpr auto is_oob(float const x, float const half_size) { return -half_size > x || x > half_size; }
} // namespace

Projectile::Projectile(NotNull<Layout const*> layout, Config config, glm::vec2 const muzzle_position) : m_layout(layout), m_config(std::move(config)) {
	m_sprite.set_texture(std::move(m_config.texture));
	m_sprite.set_auto_size(m_config.size);
	m_sprite.tint = m_config.tint;
	m_sprite.transform.position = muzzle_position + glm::vec2{0.5f * m_config.size.x, 0.0f};
}

void Projectile::tick(State const& state, Seconds const dt) {
	if (m_destroyed) { return; }

	auto const dx = m_config.x_speed * dt.count();
	m_sprite.transform.position.x += dx;

	for (auto target : state.targets) {
		if (target->get_instigator() == m_config.instigator) { continue; }
		if (check_hit(m_sprite.transform.position, m_config.size, dx, target->get_bounds())) {
			if (target->take_damage(m_config.damage)) {
				m_destroyed = true;
				break;
			}
		}
	}

	if (is_oob(m_sprite.transform.position.x, 0.5f * (m_layout->world_space.x + m_config.size.x))) { m_destroyed = true; }
}

void Projectile::draw(Shader& shader) const { m_sprite.draw(shader); }
} // namespace spaced
