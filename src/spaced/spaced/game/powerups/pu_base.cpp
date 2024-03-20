#include <spaced/game/powerups/pu_base.hpp>

namespace spaced {
using bave::Seconds;
using bave::Shader;

PUBase::PUBase(Services const& services, std::string_view const name) : m_services(&services), m_layout(&services.get<ILayout>()), m_name(name) {}

void PUBase::tick(Seconds const dt) {
	shape.transform.position.x -= speed * dt.count();
	if (shape.transform.position.x < m_layout->get_play_area().lt.x - 0.5f * shape.get_shape().diameter) { m_destroyed = true; }
}

void PUBase::draw(Shader& shader) const { shape.draw(shader); }

void PUBase::activate(Player& player) {
	do_activate(player);
	m_destroyed = true;
}
} // namespace spaced
