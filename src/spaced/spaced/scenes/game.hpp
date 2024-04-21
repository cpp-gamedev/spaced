#pragma once
#include <spaced/game/enemy_spawner.hpp>
#include <spaced/game/hud.hpp>
#include <spaced/game/scorer.hpp>
#include <spaced/game/target_provider.hpp>
#include <spaced/game/world.hpp>
#include <spaced/scene.hpp>

namespace spaced {
class Game : public Scene, public IScorer {
  public:
	Game(bave::App& app, Services const& services);

  private:
	void start_loads() final;
	void on_loaded() final;

	void on_focus(bave::FocusChange const& focus_change) final;
	void on_key(bave::KeyInput const& key_input) final;
	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;

	void tick(bave::Seconds dt) final;
	void render(bave::Shader& shader) const final;

	[[nodiscard]] auto get_score() const -> std::int64_t final { return m_score; }
	void add_score(std::int64_t score) final;

	void inspect(bave::Seconds dt, bave::Seconds frame_time);

	World m_world;
	WorldSpec m_world_spec{};
	std::int64_t m_score{};
	bave::Ptr<Hud> m_hud{};

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
