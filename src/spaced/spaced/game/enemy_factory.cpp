#include <bave/core/random.hpp>
#include <bave/json_io.hpp>
#include <spaced/game/enemies/creep.hpp>
#include <spaced/game/enemy_factory.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::NotNull;
using bave::random_in_range;

EnemyFactory::EnemyFactory(NotNull<Services const*> services, NotNull<IScorer*> scorer) : m_services(services), m_scorer(scorer) {}

auto EnemyFactory::build(dj::Json const& json) const -> std::function<std::unique_ptr<Enemy>()> {
	auto const type = json["type"].as_string();

	if (type == BasicCreepFactory::type_v) { return BasicCreepFactory{m_services, m_scorer, json}; }

	return build_default();
}

auto EnemyFactory::build_default() const -> std::function<std::unique_ptr<Enemy>()> {
	auto ret = BasicCreepFactory{m_services, m_scorer, {}};
	ret.tints = {"orange", "milk"};
	return ret;
}

BasicCreepFactory::BasicCreepFactory(NotNull<Services const*> services, NotNull<IScorer*> scorer, dj::Json const& json) : services(services), scorer(scorer) {
	for (auto const& tint : json["tints"].array_view()) { tints.push_back(tint.as<std::string>()); }
}

auto BasicCreepFactory::operator()() const -> std::unique_ptr<Enemy> {
	auto ret = std::make_unique<Creep>(*services, scorer);
	if (!tints.empty()) {
		auto const& rgbas = services->get<Styles>().rgbas;
		auto const tint_index = random_in_range(std::size_t{}, tints.size() - 1);
		ret->shape.tint = rgbas[tints.at(tint_index)];
	}
	return ret;
}
} // namespace spaced
