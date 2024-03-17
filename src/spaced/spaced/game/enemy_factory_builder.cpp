#include <bave/core/random.hpp>
#include <bave/json_io.hpp>
#include <spaced/game/enemies/creep.hpp>
#include <spaced/game/enemy_factory_builder.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::NotNull;
using bave::random_in_range;

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

BasicCreepFactory::BasicCreepFactory(NotNull<Services const*> services, NotNull<IScorer*> scorer, dj::Json const& json) : services(services), scorer(scorer) {
	for (auto const& tint : json["tints"].array_view()) { tints.push_back(tint.as<std::string>()); }
	if (auto const in_death_emitter = services->get<Resources>().get<bave::ParticleEmitter>(json["death_emitter"].as_string())) {
		death_emitter = *in_death_emitter;
	}
}

auto BasicCreepFactory::spawn_enemy() -> std::unique_ptr<Enemy> {
	auto ret = std::make_unique<Creep>(*services, scorer);
	if (!tints.empty()) {
		auto const& rgbas = services->get<Styles>().rgbas;
		auto const tint_index = random_in_range(std::size_t{}, tints.size() - 1);
		ret->shape.tint = rgbas[tints.at(tint_index)];
	}
	return ret;
}
} // namespace spaced
