#pragma once
#include <spaced/game/player.hpp>
#include <spaced/scene.hpp>

namespace spaced {
class Game : public Scene {
  public:
	Game(bave::App& app, Services const& services);

  private:
	void on_focus(bave::FocusChange const& focus_change) final;
	void on_key(bave::KeyInput const& key_input) final;
	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;

	void tick(bave::Seconds dt) final;
	void render(bave::Shader& shader) const final;

	void inspect(bave::Seconds dt, bave::Seconds frame_time);

	Player m_player;
	std::vector<bave::NotNull<IDamageable*>> m_targets{};

	struct {
		struct {
			int limit{};
			bool lock{};

			bave::Seconds elapsed{};
			int frames{};
			int fps{};

			void tick(bave::Seconds dt) {
				elapsed += dt;
				++frames;
				if (elapsed >= 1s) {
					fps = frames;
					frames = 0;
					elapsed = 0s;
				}
			}
		} fps{};
	} m_debug{};
};
} // namespace spaced
