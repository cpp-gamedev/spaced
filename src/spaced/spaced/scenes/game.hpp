#pragma once
#include <spaced/assets/asset_manifest.hpp>
#include <spaced/game/game_save.hpp>
#include <spaced/game/hud.hpp>
#include <spaced/game/scorer.hpp>
#include <spaced/game/target_provider.hpp>
#include <spaced/game/world.hpp>
#include <spaced/scene.hpp>

namespace spaced {
class GameScene : public Scene, public IScorer {
  public:
	static auto get_manifest() -> AssetManifest;

	GameScene(bave::App& app, Services const& services);

  private:
	void on_focus(bave::FocusChange const& focus_change) final;
	void on_key(bave::KeyInput const& key_input) final;
	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;

	void tick(bave::Seconds dt) final;
	void render(bave::Shader& shader) const final;

	[[nodiscard]] auto get_music_uri() const -> std::string_view final { return "music/game.mp3"; }

	[[nodiscard]] auto get_score() const -> std::int64_t final { return m_score; }
	void add_score(std::int64_t score) final;
	void on_game_over();

	void update_hi_score();

	void inspect(bave::Seconds dt, bave::Seconds frame_time);

	GameSave m_save;
	World m_world;
	std::int64_t m_score{};
	bave::Ptr<Hud> m_hud{};
	bool m_game_over_dialog_pushed{};

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
