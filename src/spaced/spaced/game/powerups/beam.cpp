#include <bave/services/resources.hpp>
#include <bave/services/styles.hpp>
#include <spaced/game/player.hpp>
#include <spaced/game/powerups/beam.hpp>
#include <spaced/game/weapons/gun_beam.hpp>

namespace spaced::powerup {
using bave::Resources;
using bave::Services;
using bave::Styles;
using bave::Texture;

Beam::Beam(Services const& services, int rounds) : Powerup(services, "Beam"), m_rounds(rounds) {
	emitter.config.lerp.tint.lo = emitter.config.lerp.tint.hi = services.get<Styles>().rgbas["gun_beam"];
	emitter.config.lerp.tint.hi.channels.w = 0;

	auto const& resources = services.get<Resources>();
	sprite.set_texture(resources.get<Texture>("assets/images/powerup_beam.png"));
}

void Beam::do_activate(Player& player) {
	auto beam = std::make_unique<GunBeam>(*m_services);
	beam->rounds = m_rounds;
	player.set_special_weapon(std::move(beam));
}
} // namespace spaced::powerup
