#include <spaced/game/arsenal.hpp>
#include <spaced/services/game_signals.hpp>
#include <spaced/services/stats.hpp>

namespace spaced {
using bave::Seconds;
using bave::Services;
using bave::Shader;

Arsenal::Arsenal(Services const& services)
	: m_stats(&services.get<Stats>()), m_weapon_changed_signal(&services.get<GameSignals>().weapon_changed), m_primary(services) {
	m_weapon_changed_signal->dispatch(get_weapon());
}

auto Arsenal::get_weapon() const -> Weapon const& {
	if (m_special) { return *m_special; }
	return m_primary;
}

auto Arsenal::get_weapon() -> Weapon& {
	return const_cast<Weapon&>(std::as_const(*this).get_weapon()); // NOLINT(cppcoreguidelines-pro-type-const-cast)
}

void Arsenal::tick(IWeaponRound::State const& round_state, bool const fire, Seconds const dt) {
	tick_weapons(dt);
	check_switch_weapon();
	if (round_state.in_play && fire) { fire_weapon(round_state.muzzle_position); }
	tick_rounds(round_state, dt);
}

void Arsenal::draw(Shader& shader) const {
	for (auto const& round : m_rounds) { round->draw(shader); }
}

void Arsenal::tick_weapons(Seconds const dt) {
	m_primary.tick(dt);

	if (m_special) {
		m_special->tick(dt);
		// if the special weapon has no more rounds and is idle, reset it.
		if (m_special->get_rounds_remaining() == 0 && m_special->is_idle()) {
			m_special.reset();
			m_weapon_changed_signal->dispatch(get_weapon());
		}
	}
}

void Arsenal::check_switch_weapon() {
	// if there is a next weapon on standby and the current weapon is idle, switch to the next weapon.
	if (m_next && get_weapon().is_idle()) {
		m_special = std::move(m_next);
		m_weapon_changed_signal->dispatch(get_weapon());
	}
}

void Arsenal::fire_weapon(glm::vec2 const muzzle_position) {
	if (auto round = get_weapon().fire(muzzle_position)) {
		m_rounds.push_back(std::move(round));
		++m_stats->player.shots_fired;
	}
}

void Arsenal::tick_rounds(IWeaponRound::State const& round_state, Seconds const dt) {
	for (auto const& round : m_rounds) { round->tick(round_state, dt); }
	std::erase_if(m_rounds, [](auto const& round) { return round->is_destroyed(); });
}
} // namespace spaced
