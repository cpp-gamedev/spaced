#pragma once
#include <spaced/game/powerup.hpp>

namespace spaced::powerup {
class Beam : public Powerup {
  public:
	explicit Beam(bave::Services const& services, int rounds = 2);

  private:
	void do_activate(Player& player) final;

	int m_rounds{};
};
} // namespace spaced::powerup
