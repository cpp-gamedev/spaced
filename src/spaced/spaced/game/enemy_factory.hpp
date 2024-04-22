#pragma once
#include <bave/graphics/particle_system.hpp>
#include <spaced/game/enemy.hpp>

namespace spaced {
class IAudio;

class EnemyFactory : public bave::Polymorphic {
  public:
	explicit EnemyFactory(bave::NotNull<Services const*> services);

	[[nodiscard]] virtual auto spawn_enemy() -> std::unique_ptr<Enemy> = 0;

	auto tick(bave::Seconds dt) -> std::unique_ptr<Enemy>;

	[[nodiscard]] auto get_services() const -> Services const& { return *m_services; }

	bave::Seconds spawn_rate{1s};

  private:
	bave::NotNull<Services const*> m_services;
	bave::NotNull<IAudio*> m_audio;

	bave::Seconds m_elapsed{};
};
} // namespace spaced
