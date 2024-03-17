#include <imgui.h>
#include <bave/core/random.hpp>
#include <bave/json_io.hpp>
#include <spaced/game/enemies/basic_creep_factory.hpp>
#include <spaced/game/enemies/creep.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::NotNull;
using bave::ParticleEmitter;
using bave::random_in_range;
using bave::Seconds;

BasicCreepFactory::BasicCreepFactory(NotNull<Services const*> services, NotNull<IScorer*> scorer, dj::Json const& json)
	: m_services(services), m_scorer(scorer) {
	for (auto const& tint : json["tints"].array_view()) { tints.push_back(tint.as<std::string>()); }
	if (auto const in_death_emitter = services->get<Resources>().get<ParticleEmitter>(json["death_emitter"].as_string())) { death_emitter = *in_death_emitter; }
	spawn_rate = Seconds{json["spawn_rate"].as<float>(spawn_rate.count())};
}

auto BasicCreepFactory::spawn_enemy() -> std::unique_ptr<Enemy> {
	auto ret = std::make_unique<Creep>(*m_services, m_scorer);
	if (!tints.empty()) {
		auto const& rgbas = m_services->get<Styles>().rgbas;
		auto const tint_index = random_in_range(std::size_t{}, tints.size() - 1);
		ret->shape.tint = rgbas[tints.at(tint_index)];
	}
	return ret;
}

auto BasicCreepFactory::tick(Seconds const dt) -> bool {
	if (spawn_rate <= 0s) { return false; }

	m_elapsed += dt;
	if (m_elapsed >= spawn_rate) {
		m_elapsed = 0s;
		return true;
	}
	return false;
}

void BasicCreepFactory::do_inspect() {
	if constexpr (bave::imgui_v) {
		auto sr = spawn_rate.count();
		if (ImGui::DragFloat("spawn rate", &sr, 0.25f, 0.25f, 10.0f)) { spawn_rate = Seconds{sr}; }

		ImGui::Separator();
	}
}
} // namespace spaced
