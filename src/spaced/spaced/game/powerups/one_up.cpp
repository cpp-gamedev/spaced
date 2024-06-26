#include <bave/services/styles.hpp>
#include <spaced/game/player.hpp>
#include <spaced/game/powerups/one_up.hpp>

namespace spaced::powerup {
using bave::Services;
using bave::Styles;

OneUp::OneUp(Services const& services) : Powerup(services, "OneUp") {
	emitter.config.lerp.tint.lo = emitter.config.lerp.tint.hi = shape.tint = services.get<Styles>().rgbas["exhaust"];
	emitter.config.lerp.tint.hi.channels.w = 0;
}

void OneUp::do_activate(Player& player) { player.one_up(); }
} // namespace spaced::powerup
