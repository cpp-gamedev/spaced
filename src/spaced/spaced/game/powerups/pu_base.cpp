#include <bave/services/resources.hpp>
#include <spaced/game/powerups/pu_base.hpp>

namespace spaced {
using bave::ParticleEmitter;
using bave::Resources;
using bave::RoundedQuad;
using bave::Seconds;
using bave::Services;
using bave::Shader;

PUBase::PUBase(Services const& services, std::string_view const name) : m_services(&services), m_layout(&services.get<Layout>()), m_name(name) {
	auto quad = RoundedQuad{};
	quad.size = glm::vec2{40.0f};
	quad.corner_radius = 12.5f;
	shape.set_shape(quad);

	auto const& resources = services.get<Resources>();
	if (auto const pu_emitter = resources.get<ParticleEmitter>("particles/powerup.json")) { emitter = *pu_emitter; }

	emitter.config.respawn = true;
}

void PUBase::tick(Seconds const dt) {
	shape.transform.position.x -= speed * dt.count();
	if (shape.transform.position.x < m_layout->play_area.lt.x - 0.5f * shape.get_shape().size.x) { m_destroyed = true; }

	emitter.set_position(shape.transform.position);
	if (!m_emitter_ticked) {
		m_emitter_ticked = true;
		emitter.pre_warm();
	}
	emitter.tick(dt);
}

void PUBase::draw(Shader& shader) const {
	shape.draw(shader);
	emitter.draw(shader);
}

void PUBase::activate(Player& player) {
	do_activate(player);
	m_destroyed = true;
}
} // namespace spaced
