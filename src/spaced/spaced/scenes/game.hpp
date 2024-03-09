#pragma once
#include <spaced/game/player.hpp>
#include <spaced/game/scorer.hpp>
#include <spaced/game/target_provider.hpp>
#include <spaced/scene.hpp>

// temporary
#include <spaced/game/enemy.hpp>

namespace spaced {
class Game : public Scene, public ITargetProvider, public IScorer {
  public:
	Game(bave::App& app, Services const& services);

  private:
	void on_loaded() final;

	void on_focus(bave::FocusChange const& focus_change) final;
	void on_key(bave::KeyInput const& key_input) final;
	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;

	void tick(bave::Seconds dt) final;
	void render(bave::Shader& shader) const final;

	[[nodiscard]] auto get_targets() const -> std::span<bave::NotNull<IDamageable*> const> final { return m_targets; }

	[[nodiscard]] auto get_score() const -> std::int64_t final { return m_score; }
	void add_score(std::int64_t const score) final { m_score += score; }

	void inspect(bave::Seconds dt, bave::Seconds frame_time);

	Player m_player;
	std::int64_t m_score{};
	std::vector<std::unique_ptr<Enemy>> m_enemies{};
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

		bave::Seconds spawn_rate{2s};
		bave::Seconds next_spawn{spawn_rate};
	} m_debug{};

	void debug_inspect_enemies();
	void debug_spawn_creep();
	void debug_controller_type();
};
} // namespace spaced
