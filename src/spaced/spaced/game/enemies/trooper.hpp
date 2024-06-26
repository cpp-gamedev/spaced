#pragma once
#include <spaced/game/enemies/gunner_base.hpp>
#include <spaced/game/weapons/gun_kinetic.hpp>

namespace spaced::enemy {
class Trooper : public GunnerBase {
  public:
	explicit Trooper(bave::Services const& services, bave::NotNull<GunKinetic*> gun);

  private:
	void do_tick(bave::Seconds dt) final;

	float m_y_speed{150.0f};
	float m_direction{1.0f};
};
} // namespace spaced::enemy
