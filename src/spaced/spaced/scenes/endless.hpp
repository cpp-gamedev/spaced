#pragma once
#include <spaced/game/enemies/creep.hpp>
#include <spaced/scenes/game.hpp>

namespace spaced {
class EndlessScene : public GameScene {
  public:
	EndlessScene(bave::App& app, bave::Services const& services);

  private:
	template <std::derived_from<Enemy> T>
	struct SpawnTimer {
		bave::NotNull<bave::Services const*> services;
		bave::Seconds spawn_rate{1s};
		bave::Seconds elapsed{};

		auto tick(bave::Seconds const dt) -> std::unique_ptr<T> {
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

	SpawnTimer<Creep> m_creeps;
};
} // namespace spaced
