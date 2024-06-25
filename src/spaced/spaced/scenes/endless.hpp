#pragma once
#include <spaced/scenes/game.hpp>

namespace spaced {
class EndlessScene : public GameScene {
  public:
	EndlessScene(bave::App& app, bave::Services const& services);
};
} // namespace spaced
