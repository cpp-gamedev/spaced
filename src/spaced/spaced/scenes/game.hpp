#pragma once
#include <bave/scene.hpp>
#include <spaced/game/game_save.hpp>
#include <spaced/game/hud.hpp>
#include <spaced/game/player.hpp>
#include <spaced/game/target_provider.hpp>
#include <spaced/game/world.hpp>
#include <spaced/signal.hpp>

namespace spaced {
class GameScene : public bave::Scene {
  public:
	GameScene(bave::App& app, bave::Services const& services);

  protected:
	[[nodiscard]] auto is_game_over() const -> bool { return m_player->is_idle() && m_spare_lives == 0; }
	[[nodiscard]] auto is_in_play() const -> bool { return !is_game_over(); }

	auto get_asset_manifest() -> bave::AssetManifest override;
	void on_loaded() override;

	void tick(bave::Seconds dt) override;

	void push_enemy(std::unique_ptr<Enemy> enemy) { m_world.value().push(std::move(enemy)); }
	void push_powerup(std::unique_ptr<Powerup> powerup) { m_world.value().push(std::move(powerup)); }

  private:
	void on_focus(bave::FocusChange const& focus_change) final;
	void on_key(bave::KeyInput const& key_input) final;
	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;

	void render(bave::Shader& shader) const final;

	void start_play();

	void on_player_death();
	void respawn_player();
	void on_game_over();

	void update_hi_score();

	void inspect(bave::Seconds dt, bave::Seconds frame_time);

	void debug_controller_type();

	bave::NotNull<bave::IAudio*> m_audio;

	SignalHandle m_on_player_scored{};
	SignalHandle m_on_1up{};

	GameSave m_save;
	World::CreateInfo m_wci{};
	std::optional<World> m_world{};
	std::optional<Player> m_player{};
	int m_spare_lives{2};
	bool m_paused{};

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
