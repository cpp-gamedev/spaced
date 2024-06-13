#pragma once
#include <spaced/game/powerups/pu_base.hpp>

namespace spaced {
class PUBeam : public PUBase {
  public:
	explicit PUBeam(bave::Services const& services, int rounds = 2);

  private:
	void do_activate(Player& player) final;

	int m_rounds{};
};
} // namespace spaced
