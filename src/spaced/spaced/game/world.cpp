#include <bave/imgui/im_text.hpp>
#include <bave/services/resources.hpp>
#include <bave/services/styles.hpp>
#include <spaced/game/enemies/creep_factory.hpp>
#include <spaced/game/world.hpp>
#include <spaced/services/game_signals.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/stats.hpp>

#include <bave/core/random.hpp>
#include <spaced/game/powerups/pu_beam.hpp>

namespace spaced {
using bave::FixedString;
using bave::IAudio;
using bave::NotNull;
using bave::ParticleEmitter;
using bave::random_in_range;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Shader;
using bave::Styles;
using bave::Texture;

namespace {
[[nodiscard]] constexpr auto to_spawn_rate(float const starfield_density) { return Seconds{std::lerp(2.0f, 0.2f, starfield_density)}; }
} // namespace

World::World(bave::NotNull<Services const*> services, CreateInfo const& create_info)
	: m_services(services), m_resources(&services->get<Resources>()), m_audio(&services->get<IAudio>()), m_stats(&services->get<Stats>()),
	  m_on_player_scored(&services->get<GameSignals>().player_scored), m_star_field(*services) {
	m_enemy_factories["CreepFactory"] = std::make_unique<CreepFactory>(services);

	auto const& resources = services->get<Resources>();

	auto const play_area = services->get<Layout>().play_area;
	auto const& rgbas = services->get<Styles>().rgbas;
	auto quad = bave::Quad{.size = play_area.size()};
	auto geometry = quad.to_geometry();
	geometry.vertex_array.vertices[0].rgba = geometry.vertex_array.vertices[1].rgba = rgbas["bg_bottom"].to_vec4();
	geometry.vertex_array.vertices[2].rgba = geometry.vertex_array.vertices[3].rgba = rgbas["bg_top"].to_vec4();
	m_background.set_geometry(std::move(geometry));
	m_background.transform.position = play_area.centre();

	auto const config = StarField::Config{.spawn_rate = to_spawn_rate(create_info.starfield_density)};
	m_star_field.add_field(resources.get<Texture>("images/star_blue.png"), config);
	m_star_field.add_field(resources.get<Texture>("images/star_red.png"), config);
	m_star_field.add_field(resources.get<Texture>("images/star_yellow.png"), config);
}

void World::tick(Seconds const dt, bool const in_play) {
	if (in_play) {
		m_star_field.tick(dt);
		for (auto& [_, factory] : m_enemy_factories) {
			if (auto enemy = factory->tick(dt)) { m_active_enemies.push_back(std::move(enemy)); }
		}
	}

	for (auto const& enemy : m_active_enemies) {
		enemy->tick(dt, in_play);
		if (enemy->is_dead()) { on_death(*enemy, true); }
	}
	std::erase_if(m_active_enemies, [](auto const& enemy) { return enemy->is_destroyed(); });

	for (auto& emitter : m_enemy_death_emitters) { emitter.tick(dt); }
	std::erase_if(m_enemy_death_emitters, [](ParticleEmitter const& emitter) { return emitter.active_particles() == 0; });

	if (!in_play) { m_active_powerups.clear(); }
	for (auto const& powerup : m_active_powerups) { powerup->tick(dt); }
	std::erase_if(m_active_powerups, [](auto const& powerup) { return powerup->is_destroyed(); });

	m_targets.clear();
	for (auto const& enemy : m_active_enemies) { m_targets.emplace_back(enemy.get()); }
	m_powerups.clear();
	for (auto const& powerup : m_active_powerups) { m_powerups.emplace_back(powerup.get()); }
}

void World::draw(Shader& shader) const {
	m_background.draw(shader);
	m_star_field.draw(shader);
	for (auto const& enemy : m_active_enemies) { enemy->draw(shader); }
	for (auto const& emitter : m_enemy_death_emitters) { emitter.draw(shader); }
	for (auto const& powerup : m_active_powerups) { powerup->draw(shader); }
}

void World::on_death(Enemy const& enemy, bool const add_score) {
	if (auto source = m_resources->get<ParticleEmitter>(enemy.death_emitter)) {
		auto& emitter = m_enemy_death_emitters.emplace_back(*source);
		emitter.config.respawn = false;
		emitter.set_position(enemy.sprite.transform.position);
	}

	m_audio->play_any_sfx(enemy.death_sfx);

	if (add_score) {
		m_on_player_scored->dispatch(enemy.points);
		++m_stats->player.enemies_poofed;

		// temp
		if (random_in_range(0, 10) < 3) { debug_spawn_powerup(enemy.sprite.transform.position); }
		// temp
	}
}

void World::do_inspect() {
	if constexpr (bave::imgui_v) {
		if (ImGui::BeginTabItem("Enemies")) {
			inspect_enemies();
			ImGui::EndTabItem();
		}
	}
}

void World::inspect_enemies() {
	if constexpr (bave::imgui_v) {
		for (std::size_t i = 0; i < m_active_enemies.size(); ++i) {
			if (ImGui::TreeNode(FixedString{"{}", i}.c_str())) {
				m_active_enemies.at(i)->inspect();
				ImGui::TreePop();
			}
		}
	}
}

void World::debug_spawn_powerup(glm::vec2 const position) {
	auto powerup = std::make_unique<PUBeam>(*m_services);
	powerup->shape.transform.position = position;
	m_active_powerups.push_back(std::move(powerup));
}
} // namespace spaced
