#pragma once
#include <bave/services/service.hpp>
#include <spaced/game/enemy.hpp>
#include <spaced/game/weapon.hpp>
#include <spaced/signal.hpp>

namespace spaced {
struct SigWeaponChanged : Signal<void(Weapon)> {};
struct SigPlayerScored : Signal<void(Enemy)> {};
struct Sig1up : Signal<void()> {};

class GameSignals : public bave::IService {
  public:
	SigWeaponChanged weapon_changed{};
	SigPlayerScored player_scored{};
	Sig1up one_up{};
};
} // namespace spaced
