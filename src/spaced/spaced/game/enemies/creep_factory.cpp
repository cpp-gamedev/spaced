#include <bave/core/random.hpp>
#include <spaced/game/enemies/creep.hpp>
#include <spaced/game/enemies/creep_factory.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::random_in_range;
using bave::Seconds;

auto CreepFactory::spawn_enemy() -> std::unique_ptr<Enemy2> {
	auto ret = std::make_unique<Creep2>(get_services());
	if (!m_tints.empty()) {
		auto const& rgbas = get_services().get<Styles>().rgbas;
		auto const tint_index = random_in_range(std::size_t{}, m_tints.size() - 1);
		ret->shape.tint = rgbas[m_tints.at(tint_index)];
	}
	ret->health = m_initial_health;
	return ret;
}
} // namespace spaced
