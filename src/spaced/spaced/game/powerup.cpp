#include <bave/services/resources.hpp>
#include <spaced/game/powerup.hpp>

namespace spaced {
using bave::Circle;
using bave::IAudio;
using bave::ParticleEmitter;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Shader;
using bave::Texture;

Powerup::Powerup(Services const& services, std::string_view const name)
	: m_services(&services), m_layout(&services.get<Layout>()), m_audio(&services.get<IAudio>()), m_name(name) {

	sprite.set_size({80., 80.});

	auto const& resources = services.get<Resources>();
	if (auto const pu_emitter = resources.get<ParticleEmitter>("assets/particles/powerup.json")) { emitter = *pu_emitter; }

	emitter.config.respawn = true;
}

void Powerup::tick(Seconds const dt) {
	sprite.transform.position.x -= speed * dt.count();
	if (sprite.transform.position.x < m_layout->play_area.lt.x - 0.5f * sprite.get_size().x - 300.) { m_destroyed = true; }

	emitter.set_position(sprite.transform.position);
	if (!m_emitter_ticked) {
		m_emitter_ticked = true;
		emitter.pre_warm();
	}
	emitter.tick(dt);
}

void Powerup::draw(Shader& shader) const {
	emitter.draw(shader);
	sprite.draw(shader);
}

void Powerup::activate(Player& player) {
	do_activate(player);
	m_audio->play_sfx("assets/sfx/powerup_collect.wav");
	m_destroyed = true;
}
} // namespace spaced
