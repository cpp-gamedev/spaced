#pragma once
#include <bave/services/service.hpp>
#include <spaced/game/weapon.hpp>
#include <spaced/signal.hpp>

namespace spaced {
struct SigWeaponChanged : Signal<void(Weapon)> {};
struct SigPlayerScored : Signal<void(std::int64_t)> {};

class GameSignals : public bave::IService {
  public:
	SigWeaponChanged weapon_changed{};
	SigPlayerScored player_scored{};
};
} // namespace spaced
