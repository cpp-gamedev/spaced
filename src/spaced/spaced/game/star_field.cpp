#include <bave/core/random.hpp>
#include <bave/services/resources.hpp>
#include <spaced/game/star_field.hpp>
#include <cassert>
#include <cmath>

namespace spaced {
using bave::IDisplay;
using bave::random_in_range;
using bave::Seconds;
using bave::Services;
using bave::Shader;
using bave::Texture;

StarField::StarField(Services const& services) : m_display(&services.get<IDisplay>()) {}

void StarField::add_field(std::shared_ptr<Texture const> texture, Config const& config) {
	auto& field = m_fields.emplace_back();
	field.sprite.set_size(glm::vec2{20.0f});
	field.sprite.set_texture(std::move(texture));
	field.config = config;
	field.pre_warm(m_display->get_window_size());
}

void StarField::tick(Seconds const dt) {
	for (auto& field : m_fields) { field.tick(m_display->get_world_space(), dt); }
}

void StarField::draw(Shader& shader) const {
	for (auto const& field : m_fields) { field.sprite.draw(shader); }
}

auto StarField::Field::spawn(glm::vec2 const world_space) const -> Star {
	auto ret = Star{};

	auto const sprite_size = sprite.get_size();
	ret.position.x = 0.5f * (world_space.x + sprite_size.x);
	auto const y_max = 0.5f * (world_space.y - sprite_size.y);
	ret.position.y = random_in_range(-y_max, y_max);

	auto const alpha = random_in_range(0.0f, 1.0f);
	ret.scale = std::lerp(config.scale.lo, config.scale.hi, alpha);
	ret.speed = std::lerp(config.speed.lo, config.speed.hi, alpha);

	return ret;
}

void StarField::Field::pre_warm(glm::vec2 const world_space) {
	auto const dt = 0.5f * config.spawn_rate;
	while (true) {
		if (tick_stars(world_space, dt)) { return; }
		tick_spawn(world_space, dt);
	}
}

void StarField::Field::tick(glm::vec2 const world_space, Seconds const dt) {
	tick_stars(world_space, dt);
	tick_spawn(world_space, dt);
	sync();

	assert(sprite.instances.size() < 10000);
}

auto StarField::Field::tick_stars(glm::vec2 const world_space, Seconds const dt) -> bool {
	for (auto& star : stars) { star.position.x -= star.speed * dt.count(); }

	auto const sprite_size_x = sprite.get_size().x;
	auto const func = [sprite_size_x, world_space](Star const& s) {
		auto const exit_x = -0.5f * (world_space.x + sprite_size_x * s.scale);
		return s.position.x < exit_x;
	};
	return std::erase_if(stars, func) > 0;
}

void StarField::Field::tick_spawn(glm::vec2 const world_space, Seconds const dt) {
	next_spawn -= dt;
	if (next_spawn <= 0s) {
		next_spawn = config.spawn_rate;
		stars.push_back(spawn(world_space));
	}
}

void StarField::Field::sync() {
	sprite.instances.clear();
	sprite.instances.reserve(stars.size());
	for (auto const& star : stars) {
		auto& render_instance = sprite.instances.emplace_back();
		render_instance.transform.position = star.position;
		render_instance.transform.scale = glm::vec2{star.scale};
		render_instance.tint = star.tint;
	}
}
} // namespace spaced
