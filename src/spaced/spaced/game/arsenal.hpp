#pragma once
#include <spaced/game/weapons/gun_kinetic.hpp>

namespace spaced {
struct Stats;
struct SigWeaponChanged;

// Arsenal models a main/primary weapon, and an possible special weapon.
// Weapons only switch when they are idle.
class Arsenal {
  public:
	explicit Arsenal(bave::Services const& services);

	[[nodiscard]] auto get_weapon() const -> Weapon const&;
	[[nodiscard]] auto get_weapon() -> Weapon&;

	[[nodiscard]] auto is_special_active() const -> bool { return m_special != nullptr; }

	void set_special(std::unique_ptr<Weapon> weapon) { m_next = std::move(weapon); }

	// returns whether a round was fired this frame
	auto tick(IWeaponRound::State const& round_state, bool fire, bave::Seconds dt) -> bool;
	void draw(bave::Shader& shader) const;

  private:
	void tick_weapons(bave::Seconds dt);
	void check_switch_weapon();
	auto fire_weapon(glm::vec2 muzzle_position) -> bool;
	void tick_rounds(IWeaponRound::State const& round_state, bave::Seconds dt);

	bave::NotNull<Stats*> m_stats;
	bave::NotNull<SigWeaponChanged*> m_weapon_changed_signal;

	GunKinetic m_primary;				 // main weapon
	std::unique_ptr<Weapon> m_special{}; // special weapon
	std::unique_ptr<Weapon> m_next{};	 // next special weapon (on standby until current weapon is idle)
	std::vector<std::unique_ptr<Weapon::Round>> m_rounds{};
};
} // namespace spaced
