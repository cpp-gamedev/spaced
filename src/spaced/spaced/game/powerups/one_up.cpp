#include <bave/services/resources.hpp>
#include <bave/services/styles.hpp>
#include <spaced/game/player.hpp>
#include <spaced/game/powerups/one_up.hpp>

namespace spaced::powerup {
using bave::Resources;
using bave::Services;
using bave::Styles;
using bave::Texture;

OneUp::OneUp(Services const& services) : Powerup(services, "OneUp") {
	emitter.config.lerp.tint.lo = emitter.config.lerp.tint.hi = services.get<Styles>().rgbas["exhaust"];
	emitter.config.lerp.tint.hi.channels.w = 0;

	auto const& resources = services.get<Resources>();
	sprite.set_texture(resources.get<Texture>("assets/images/extra_life.png"));
}

void OneUp::do_activate(Player& player) { player.one_up(); }
} // namespace spaced::powerup
