#include <spaced/game/enemies/basic_creep_factory.hpp>
#include <spaced/game/enemy_factory_builder.hpp>

namespace spaced {
using bave::NotNull;

EnemyFactoryBuilder::EnemyFactoryBuilder(NotNull<Services const*> services, NotNull<IScorer*> scorer) : m_services(services), m_scorer(scorer) {}

auto EnemyFactoryBuilder::build(dj::Json const& json) const -> std::unique_ptr<IEnemyFactory> {
	auto const type = json["type"].as_string();

	if (type == BasicCreepFactory::type_v) { return std::make_unique<BasicCreepFactory>(m_services, m_scorer, json); }

	return build_default();
}

auto EnemyFactoryBuilder::build_default() const -> std::unique_ptr<IEnemyFactory> {
	auto ret = std::make_unique<BasicCreepFactory>(m_services, m_scorer, dj::Json{});
	ret->tints = {"orange", "milk"};
	return ret;
}
} // namespace spaced
