#include <bave/core/random.hpp>
#include <bave/imgui/im_text.hpp>
#include <spaced/game/enemy.hpp>

namespace spaced {
using bave::im_text;
using bave::random_in_range;
using bave::RoundedQuad;

Enemy::Enemy(Services const& services, std::string_view const type) : m_layout(&services.get<ILayout>()), m_type(type) {
	static constexpr auto init_size_v = glm::vec2{100.0f};
	auto const y_bounds = 0.5f * (get_layout().get_world_space().y - 0.5f * init_size_v.y);
	setup(init_size_v, random_in_range(-y_bounds, y_bounds));
}

auto Enemy::take_damage(float const damage) -> bool {
	if (is_destroyed()) { return false; }
	health.inflict_damage(damage);
	return true;
}

void Enemy::setup(glm::vec2 max_size, float y_position) {
	auto rounded_quad = RoundedQuad{};
	rounded_quad.size = max_size;
	rounded_quad.corner_radius = 0.2f * max_size.x;
	shape.set_shape(rounded_quad);
	shape.transform.position.x = 0.5f * (get_layout().get_world_space().x + rounded_quad.size.x);
	shape.transform.position.y = y_position;
}

void Enemy::do_inspect() {
	im_text("type: {}", m_type);
	health.inspect();
}
} // namespace spaced
