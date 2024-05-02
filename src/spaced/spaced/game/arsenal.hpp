#pragma once
#include <spaced/game/weapons/gun_kinetic.hpp>

namespace spaced {
struct Stats;

// Arsenal models a main/primary weapon, and an possible special weapon.
// Weapons only switch when they are idle.
class Arsenal {
  public:
	explicit Arsenal(Services const& services);

	[[nodiscard]] auto get_weapon() const -> Weapon const&;
	[[nodiscard]] auto get_weapon() -> Weapon&;

	[[nodiscard]] auto is_special_active() const -> bool { return m_special != nullptr; }

	void set_special(std::unique_ptr<Weapon> weapon) { m_next = std::move(weapon); }

	void tick(IWeaponRound::State const& round_state, bool fire, bave::Seconds dt);
	void draw(bave::Shader& shader) const;

  private:
	void tick_weapons(bave::Seconds dt);
	void check_switch_weapon();
	void fire_weapon(glm::vec2 muzzle_position);
	void tick_rounds(IWeaponRound::State const& round_state, bave::Seconds dt);

	GunKinetic m_primary; // main weapon
	bave::NotNull<Stats*> m_stats;
	std::unique_ptr<Weapon> m_special{}; // special weapon
	std::unique_ptr<Weapon> m_next{};	 // next special weapon (on standby until current weapon is idle)
	std::vector<std::unique_ptr<Weapon::Round>> m_rounds{};
};
} // namespace spaced
