#pragma once
#include <djson/json.hpp>
#include <spaced/game/enemy_factory.hpp>

namespace spaced {
class EnemyFactoryBuilder {
  public:
	explicit EnemyFactoryBuilder(bave::NotNull<Services const*> services, bave::NotNull<IScorer*> scorer);

	[[nodiscard]] auto build(dj::Json const& json) const -> std::unique_ptr<IEnemyFactory>;

	[[nodiscard]] auto build_default() const -> std::unique_ptr<IEnemyFactory>;

  private:
	bave::NotNull<Services const*> m_services;
	bave::NotNull<IScorer*> m_scorer;
};

struct BasicCreepFactory : IEnemyFactory {
	static constexpr std::string_view type_v{"BasicCreepFactory"};

	bave::NotNull<Services const*> services;
	bave::NotNull<IScorer*> scorer;
	std::vector<std::string> tints{};
	bave::ParticleEmitter death_emitter{};

	explicit BasicCreepFactory(bave::NotNull<Services const*> services, bave::NotNull<IScorer*> scorer, dj::Json const& json);

	[[nodiscard]] auto get_type_name() const -> std::string_view final { return type_v; }
	[[nodiscard]] auto spawn_enemy() -> std::unique_ptr<Enemy> final;
	[[nodiscard]] auto get_death_emitter() const -> bave::ParticleEmitter const& final { return death_emitter; }
};
} // namespace spaced
