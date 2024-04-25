#include <spaced/game/player.hpp>
#include <spaced/game/powerups/pu_beam.hpp>
#include <spaced/game/weapons/gun_beam.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
PUBeam::PUBeam(Services const& services, int rounds) : PUBase(services, "Beam"), m_rounds(rounds) {
	emitter.config.lerp.tint.lo = emitter.config.lerp.tint.hi = shape.tint = services.get<Styles>().rgbas["gun_beam"];
	emitter.config.lerp.tint.hi.channels.w = 0;
}

void PUBeam::do_activate(Player& player) {
	auto beam = std::make_unique<GunBeam>(*m_services);
	beam->rounds = m_rounds;
	player.set_special_weapon(std::move(beam));
}
} // namespace spaced
