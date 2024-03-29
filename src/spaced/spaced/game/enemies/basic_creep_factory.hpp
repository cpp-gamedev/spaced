#pragma once
#include <djson/json.hpp>
#include <spaced/game/enemy_factory.hpp>

namespace spaced {
class BasicCreepFactory : public IEnemyFactory {
  public:
	static constexpr std::string_view type_v{"BasicCreepFactory"};

	explicit BasicCreepFactory(bave::NotNull<Services const*> services, bave::NotNull<IEnemyDeathListener*> listener, dj::Json const& json);

	[[nodiscard]] auto get_type_name() const -> std::string_view final { return type_v; }
	[[nodiscard]] auto spawn_enemy() -> std::unique_ptr<Enemy> final;
	[[nodiscard]] auto get_death_emitter() const -> bave::ParticleEmitter const& final { return death_emitter; }

	auto tick(bave::Seconds dt) -> bool final;

	std::vector<std::string> tints{};
	bave::ParticleEmitter death_emitter{};
	bave::Seconds spawn_rate{2s};

  private:
	void do_inspect() final;

	bave::NotNull<Services const*> m_services;
	bave::NotNull<IEnemyDeathListener*> m_listener;
	bave::Seconds m_elapsed{};
};
} // namespace spaced
