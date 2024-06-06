#include <bave/core/random.hpp>
#include <bave/imgui/im_text.hpp>
#include <spaced/game/enemy.hpp>
#include <spaced/services/services.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::im_text;
using bave::random_in_range;
using bave::RoundedQuad;
using bave::Seconds;
using bave::Shader;

Enemy::Enemy(Services const& services, std::string_view const type) : m_layout(&services.get<ILayout>()), m_health_bar(services), m_type(type) {
	static constexpr auto init_size_v = glm::vec2{100.0f};
	auto const play_area = m_layout->get_play_area();
	auto const y_min = play_area.rb.y + 0.5f * init_size_v.y;
	auto const y_max = play_area.lt.y - 0.5f * init_size_v.y - 50.0f;
	setup(init_size_v, random_in_range(y_min, y_max));

	m_health_bar.set_style(services.get<Styles>().progress_bars["enemy"]);
}

auto Enemy::take_damage(float const damage) -> bool {
	if (is_destroyed()) { return false; }
	health.inflict_damage(damage);
	return true;
}

void Enemy::force_death() {
	health = 0.0f;
	m_health_bar.set_progress(0.0f);
}

void Enemy::tick(Seconds const /*dt*/, bool const /*in_play*/) {
	m_health_bar.position = shape.transform.position;
	m_health_bar.position.y += 0.5f * shape.get_shape().size.y + 20.0f;
	m_health_bar.size = {shape.get_shape().size.x, 10.0f};
	m_health_bar.set_progress(health.get_hit_points() / health.get_total_hit_points());
}

void Enemy::draw(Shader& shader) const {
	shape.draw(shader);
	m_health_bar.draw(shader);
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
	if constexpr (bave::imgui_v) {
		im_text("type: {}", m_type);
		health.inspect();
	}
}
} // namespace spaced
