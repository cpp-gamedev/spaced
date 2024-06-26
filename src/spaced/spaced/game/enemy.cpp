#include <bave/core/random.hpp>
#include <bave/imgui/im_text.hpp>
#include <bave/services/services.hpp>
#include <bave/services/styles.hpp>
#include <spaced/game/enemy.hpp>

namespace spaced {
using bave::im_text;
using bave::random_in_range;
using bave::Rect;
using bave::RoundedQuad;
using bave::Seconds;
using bave::Services;
using bave::Shader;
using bave::Styles;

Enemy::Enemy(Services const& services, std::string_view const type) : m_layout(&services.get<Layout>()), m_health_bar(services), m_type(type) {
	static constexpr auto init_size_v = glm::vec2{100.0f};
	auto const play_area = m_layout->play_area;
	auto const y_min = play_area.rb.y + 0.5f * init_size_v.y;
	auto const y_max = play_area.lt.y - 0.5f * init_size_v.y - 50.0f;
	setup(init_size_v, random_in_range(y_min, y_max));

	m_health_bar.set_style(services.get<Styles>().progress_bars["enemy"]);

	update_health_bar();
}

auto Enemy::get_bounds() const -> bave::Rect<> {
	if (m_hitbox) { return Rect<>::from_size(*m_hitbox, m_sprite.transform.position); }
	return m_sprite.get_bounds();
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

void Enemy::update_health_bar() {
	auto position = m_sprite.transform.position;
	position.y += 0.5f * m_sprite.get_shape().size.y + 20.0f;
	m_health_bar.set_position(position);
	m_health_bar.set_size({m_sprite.get_shape().size.x, 10.0f});
	m_health_bar.set_progress(health.get_hit_points() / health.get_total_hit_points());
}

auto Enemy::tick(Seconds const dt) -> std::unique_ptr<IWeaponRound> {
	do_tick(dt);

	m_sprite.transform.position.x -= speed * dt.count();
	if (m_sprite.transform.position.x < -0.5f * (get_layout().world_space.x + m_sprite.get_shape().size.x)) { set_destroyed(); }

	update_health_bar();

	return make_round();
}

void Enemy::draw(Shader& shader) const {
	m_sprite.draw(shader);
	m_health_bar.draw(shader);
}

void Enemy::setup(glm::vec2 max_size, float y_position) {
	auto rounded_quad = RoundedQuad{};
	rounded_quad.size = max_size;
	rounded_quad.corner_radius = 0.2f * max_size.x;
	m_sprite.set_shape(rounded_quad);
	m_sprite.transform.position.x = 0.5f * (get_layout().world_space.x + rounded_quad.size.x);
	m_sprite.transform.position.y = y_position;
}

void Enemy::do_inspect() {
	if constexpr (bave::imgui_v) {
		im_text("type: {}", m_type);
		health.inspect();
	}
}
} // namespace spaced
