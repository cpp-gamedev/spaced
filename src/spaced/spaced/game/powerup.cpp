#include <bave/services/resources.hpp>
#include <spaced/game/powerup.hpp>

namespace spaced {
using bave::Circle;
using bave::ParticleEmitter;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Shader;

Powerup::Powerup(Services const& services, std::string_view const name) : m_services(&services), m_layout(&services.get<Layout>()), m_name(name) {
	auto circle = Circle{};
	circle.diameter = 40.0f;
	shape.set_shape(circle);

	auto const& resources = services.get<Resources>();
	if (auto const pu_emitter = resources.get<ParticleEmitter>("particles/powerup.json")) { emitter = *pu_emitter; }

	emitter.config.respawn = true;
}

void Powerup::tick(Seconds const dt) {
	shape.transform.position.x -= speed * dt.count();
	if (shape.transform.position.x < m_layout->play_area.lt.x - 0.5f * shape.get_shape().diameter) { m_destroyed = true; }

	emitter.set_position(shape.transform.position);
	if (!m_emitter_ticked) {
		m_emitter_ticked = true;
		emitter.pre_warm();
	}
	emitter.tick(dt);
}

void Powerup::draw(Shader& shader) const {
	shape.draw(shader);
	emitter.draw(shader);
}

void Powerup::activate(Player& player) {
	do_activate(player);
	m_destroyed = true;
}
} // namespace spaced
