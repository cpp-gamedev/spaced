#pragma once
#include <bave/scene.hpp>
#include <spaced/game/game_save.hpp>
#include <spaced/game/hud.hpp>
#include <spaced/game/player.hpp>
#include <spaced/game/scorer.hpp>
#include <spaced/game/target_provider.hpp>
#include <spaced/game/world.hpp>

namespace spaced {
class GameScene : public bave::Scene, public IScorer {
  public:
	GameScene(bave::App& app, bave::Services const& services);

  private:
	auto get_asset_manifest() -> bave::AssetManifest final;
	void on_loaded() final;

	void on_focus(bave::FocusChange const& focus_change) final;
	void on_key(bave::KeyInput const& key_input) final;
	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;

	void tick(bave::Seconds dt) final;
	void render(bave::Shader& shader) const final;

	[[nodiscard]] auto get_score() const -> std::int64_t final { return m_score; }
	void add_score(std::int64_t score) final;

	void start_play();

	void on_player_death();
	void respawn_player();
	void on_game_over();

	void update_hi_score();

	void inspect(bave::Seconds dt, bave::Seconds frame_time);

	void debug_controller_type();

	GameSave m_save;
	std::optional<World> m_world{};
	std::optional<Player> m_player{};
	int m_spare_lives{2};

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
