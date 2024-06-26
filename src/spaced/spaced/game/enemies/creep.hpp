#pragma once
#include <spaced/game/enemy.hpp>

namespace spaced::enemy {
class Creep : public Enemy {
  public:
	explicit Creep(bave::Services const& services);

  private:
	auto tick(bave::Seconds dt) -> std::unique_ptr<IWeaponRound> final;

	float m_speed{100.0f};
};
} // namespace spaced::enemy
