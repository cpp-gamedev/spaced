#include <spaced/game/player.hpp>
#include <spaced/game/powerups/pu_beam.hpp>
#include <spaced/game/weapons/gun_beam.hpp>

namespace spaced {
void PUBeam::do_activate(Player& player) {
	auto beam = std::make_unique<GunBeam>(*m_services);
	beam->rounds = m_rounds;
	player.set_special_weapon(std::move(beam));
}
} // namespace spaced
