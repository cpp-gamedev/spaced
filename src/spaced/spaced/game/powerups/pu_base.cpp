#include <spaced/game/powerups/pu_base.hpp>

namespace spaced {
using bave::Circle;
using bave::Seconds;
using bave::Shader;

PUBase::PUBase(Services const& services, std::string_view const name) : m_services(&services), m_layout(&services.get<ILayout>()), m_name(name) {
	auto circle = Circle{.diameter = 50.0f};
	shape.set_shape(circle);
}

void PUBase::tick(Seconds const dt) {
	motion.tick(dt);
	shape.transform.position.x += motion.velocity().x * dt.count();
	if (shape.transform.position.x < m_layout->get_play_area().lt.x - 0.5f * shape.get_shape().diameter) { m_destroyed = true; }
}

void PUBase::draw(Shader& shader) const { shape.draw(shader); }

void PUBase::activate(Player& player) {
	do_activate(player);
	m_destroyed = true;
}
} // namespace spaced
