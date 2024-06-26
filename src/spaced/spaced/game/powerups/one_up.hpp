#pragma once
#include <spaced/game/powerup.hpp>

namespace spaced::powerup {
class OneUp : public Powerup {
  public:
	explicit OneUp(bave::Services const& services);

  private:
	void do_activate(Player& player) final;
};
} // namespace spaced::powerup
