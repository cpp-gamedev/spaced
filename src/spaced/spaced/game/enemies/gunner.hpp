#pragma once
#include <spaced/game/enemies/gunner_base.hpp>

namespace spaced::enemy {
class Gunner : public GunnerBase {
  public:
	explicit Gunner(bave::Services const& services, bave::NotNull<GunKinetic*> gun);
};
} // namespace spaced::enemy
