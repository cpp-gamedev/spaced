#pragma once
#include <djson/json.hpp>
#include <spaced/game/enemy.hpp>
#include <functional>

namespace spaced {
class EnemyFactory {
  public:
	explicit EnemyFactory(bave::NotNull<Services const*> services, bave::NotNull<IScorer*> scorer);

	[[nodiscard]] auto build(dj::Json const& json) const -> std::function<std::unique_ptr<Enemy>()>;

	[[nodiscard]] auto build_default() const -> std::function<std::unique_ptr<Enemy>()>;

  private:
	bave::NotNull<Services const*> m_services;
	bave::NotNull<IScorer*> m_scorer;
};

struct BasicCreepFactory {
	static constexpr std::string_view type_v{"BasicCreepFactory"};

	bave::NotNull<Services const*> services;
	bave::NotNull<IScorer*> scorer;
	std::vector<std::string> tints{};

	explicit BasicCreepFactory(bave::NotNull<Services const*> services, bave::NotNull<IScorer*> scorer, dj::Json const& json);

	[[nodiscard]] auto operator()() const -> std::unique_ptr<Enemy>;
};
} // namespace spaced
