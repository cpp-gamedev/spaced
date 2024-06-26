#include <bave/services/styles.hpp>
#include <spaced/game/player.hpp>
#include <spaced/game/powerups/beam.hpp>
#include <spaced/game/weapons/gun_beam.hpp>

namespace spaced::powerup {
using bave::Services;
using bave::Styles;

Beam::Beam(Services const& services, int rounds) : Powerup(services, "Beam"), m_rounds(rounds) {
	emitter.config.lerp.tint.lo = emitter.config.lerp.tint.hi = shape.tint = services.get<Styles>().rgbas["gun_beam"];
	emitter.config.lerp.tint.hi.channels.w = 0;
}

void Beam::do_activate(Player& player) {
	auto beam = std::make_unique<GunBeam>(*m_services);
	beam->rounds = m_rounds;
	player.set_special_weapon(std::move(beam));
}
} // namespace spaced::powerup
