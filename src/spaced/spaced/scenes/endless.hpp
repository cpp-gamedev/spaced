#pragma once
#include <spaced/scenes/game.hpp>
#include <spaced/signal.hpp>

namespace spaced {
class EndlessScene : public GameScene {
  public:
	EndlessScene(bave::App& app, bave::Services const& services);

  private:
	struct ISpawnTimer : bave::Polymorphic {
		virtual auto tick(bave::Seconds dt) -> std::unique_ptr<Enemy> = 0;
	};

	template <std::derived_from<Enemy> T>
	struct SpawnTimer : ISpawnTimer {
		bave::NotNull<bave::Services const*> services;
		bave::Seconds spawn_rate{1s};
		bave::Seconds elapsed{};

		explicit SpawnTimer(bave::NotNull<bave::Services const*> services) : services(services) {}

		auto tick(bave::Seconds const dt) -> std::unique_ptr<Enemy> final {
			auto ret = std::unique_ptr<T>{};
			elapsed += dt;
			if (elapsed >= spawn_rate) {
				ret = std::make_unique<T>(*services);
				elapsed = {};
			}
			return ret;
		}
	};

	void tick(bave::Seconds dt) final;

	void debug_spawn_powerup(glm::vec2 position);

	SignalHandle m_on_player_scored{};

	std::vector<std::unique_ptr<ISpawnTimer>> m_spawn_timers{};
};
} // namespace spaced
