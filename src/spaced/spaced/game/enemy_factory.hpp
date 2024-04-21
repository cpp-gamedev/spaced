#pragma once
#include <bave/graphics/particle_system.hpp>
#include <spaced/game/enemy.hpp>
#include <string>
#include <vector>

namespace spaced {
class IAudio;

class IEnemyFactory : public bave::Polymorphic {
  public:
	explicit IEnemyFactory(bave::NotNull<Services const*> services);

	[[nodiscard]] virtual auto get_type_name() const -> std::string_view = 0;
	[[nodiscard]] virtual auto spawn_enemy() -> std::unique_ptr<Enemy> = 0;
	[[nodiscard]] virtual auto get_death_emitter() const -> bave::ParticleEmitter const& = 0;

	void play_death_sfx();

	virtual auto tick(bave::Seconds dt) -> bool = 0;

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

  protected:
	[[nodiscard]] auto get_services() const -> Services const& { return *m_services; }

	std::vector<std::string> m_death_sfx{};

  private:
	virtual void do_inspect() {}

	bave::NotNull<Services const*> m_services;
	bave::NotNull<IAudio*> m_audio;
};

class EnemyFactory : public bave::Polymorphic {
  public:
	explicit EnemyFactory(bave::NotNull<Services const*> services);

	[[nodiscard]] virtual auto spawn_enemy() -> std::unique_ptr<Enemy2> = 0;

	auto tick(bave::Seconds dt) -> std::unique_ptr<Enemy2>;

	[[nodiscard]] auto get_services() const -> Services const& { return *m_services; }

	bave::Seconds spawn_rate{1s};

  private:
	bave::NotNull<Services const*> m_services;
	bave::NotNull<IAudio*> m_audio;

	bave::Seconds m_elapsed{};
};
} // namespace spaced
