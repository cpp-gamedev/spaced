#pragma once
#include <spaced/game/enemy.hpp>
#include <spaced/game/weapons/gun_kinetic.hpp>

namespace spaced::enemy {
class Trooper : public Enemy {
  public:
	explicit Trooper(bave::Services const& services, bave::NotNull<GunKinetic*> gun);

  private:
	auto make_round() -> std::unique_ptr<IWeaponRound> final;
	void do_tick(bave::Seconds dt) final;

	[[nodiscard]] auto get_muzzle_position() const -> glm::vec2;

	bave::NotNull<GunKinetic*> m_gun;
	bave::Seconds m_fire_rate{3.3s};

	bave::Seconds m_fire_elapsed{};
};
} // namespace spaced::enemy
