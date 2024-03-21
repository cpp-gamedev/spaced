#pragma once
#include <djson/json.hpp>
#include <spaced/game/enemy_factory.hpp>

namespace spaced {
class EnemyFactoryBuilder {
  public:
	explicit EnemyFactoryBuilder(bave::NotNull<Services const*> services, bave::NotNull<IEnemyDeathListener*> listener);

	[[nodiscard]] auto build(dj::Json const& json) const -> std::unique_ptr<IEnemyFactory>;

	[[nodiscard]] auto build_default() const -> std::unique_ptr<IEnemyFactory>;

  private:
	bave::NotNull<Services const*> m_services;
	bave::NotNull<IEnemyDeathListener*> m_listener;
};
} // namespace spaced
