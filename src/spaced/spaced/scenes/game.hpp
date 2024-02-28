#pragma once
#include <spaced/game/player.hpp>
#include <spaced/scene.hpp>

namespace spaced {
class Game : public Scene {
  public:
	Game(bave::App& app, Services const& services);

  private:
	struct Fps {
		int limit{};

		bave::Seconds elapsed{};
		int frames{};
		int fps{};

		void tick(bave::Seconds dt);
	};

	void on_key(bave::KeyInput const& key_input) final;
	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;

	void tick(bave::Seconds dt) final;
	void render(bave::Shader& shader) const final;

	Player m_player;

	Fps m_fps{};
};
} // namespace spaced
