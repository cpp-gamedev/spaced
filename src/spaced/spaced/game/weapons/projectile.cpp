#include <spaced/game/weapons/projectile.hpp>

namespace spaced {
using bave::NotNull;
using bave::Rect;
using bave::RoundedQuad;
using bave::Seconds;
using bave::Shader;

namespace {
[[nodiscard]] constexpr auto check_hit(glm::vec2 const position, glm::vec2 const size, float const dx, Rect<> const& target) -> bool {
	auto rect = Rect<>::from_size(size, position);
	rect.lt.x -= dx;
	return is_intersecting(rect, target);
}
} // namespace

Projectile::Projectile(NotNull<ILayout const*> layout, Config config, glm::vec2 const muzzle_position) : m_layout(layout), m_config(std::move(config)) {
	auto rounded_quad = RoundedQuad{};
	rounded_quad.size = m_config.size;
	rounded_quad.corner_radius = m_config.corner_radius;
	m_shape.set_shape(rounded_quad);

	m_shape.set_texture(std::move(m_config.texture));
	m_shape.tint = m_config.tint;
	m_shape.transform.position = muzzle_position + glm::vec2{0.5f * m_config.size.x, 0.0f};
}

void Projectile::tick(State const& state, Seconds const dt) {
	if (m_destroyed) { return; }

	auto const dx = m_config.x_speed * dt.count();
	m_shape.transform.position.x += dx;

	for (auto target : state.targets) {
		if (target->get_instigator() == m_config.instigator) { continue; }
		if (check_hit(m_shape.transform.position, m_config.size, dx, target->get_bounds())) {
			if (target->take_damage(m_config.damage)) {
				m_destroyed = true;
				break;
			}
		}
	}

	if (m_shape.transform.position.x > 0.5f * (m_layout->get_world_space().x + m_config.size.x)) { m_destroyed = true; }
}

void Projectile::draw(Shader& shader) const { m_shape.draw(shader); }
} // namespace spaced
