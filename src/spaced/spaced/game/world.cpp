#include <bave/imgui/im_text.hpp>
#include <bave/services/resources.hpp>
#include <spaced/game/enemies/creep_factory.hpp>
#include <spaced/game/world.hpp>
#include <spaced/services/stats.hpp>

#include <bave/core/random.hpp>
#include <spaced/game/controllers/auto_controller.hpp>
#include <spaced/game/controllers/player_controller.hpp>
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
using bave::Texture;

namespace {
[[nodiscard]] auto make_player_controller(Services const& services) {
	auto ret = std::make_unique<PlayerController>(services);
	if constexpr (bave::platform_v == bave::Platform::eAndroid) { ret->set_type(PlayerController::Type::eTouch); }
	auto const& layout = services.get<Layout>();
	auto const half_size = 0.5f * layout.player_size;
	auto const play_area = layout.play_area;
	ret->max_y = play_area.lt.y - half_size.y;
	ret->min_y = play_area.rb.y + half_size.y;
	return ret;
}

[[nodiscard]] auto make_auto_controller(ITargetProvider const& target_provider, Services const& services) {
	return std::make_unique<AutoController>(&target_provider, services.get<Layout>().player_x);
}
} // namespace

World::World(bave::NotNull<Services const*> services, bave::NotNull<IScorer*> scorer)
	: player(*services, make_player_controller(*services)), m_services(services), m_resources(&services->get<Resources>()), m_audio(&services->get<IAudio>()),
	  m_stats(&services->get<Stats>()), m_scorer(scorer), m_background(*services) {
	m_enemy_factories["CreepFactory"] = std::make_unique<CreepFactory>(services);

	m_background.set_texture(services->get<Resources>().get<Texture>("images/background.png"));
	m_background.set_tile_size(glm::vec2{300.0f});
	m_background.x_speed = 50.0f;
}

void World::tick(Seconds const dt) {
	bool const in_play = !player.health.is_dead();

	if (in_play) {
		m_background.tick(dt);
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

	for (auto const& powerup : m_active_powerups) { powerup->tick(dt); }
	std::erase_if(m_active_powerups, [](auto const& powerup) { return powerup->is_destroyed(); });

	m_targets.clear();
	for (auto const& enemy : m_active_enemies) { m_targets.emplace_back(enemy.get()); }
	m_powerups.clear();
	for (auto const& powerup : m_active_powerups) { m_powerups.emplace_back(powerup.get()); }

	auto const player_state = Player::State{.targets = m_targets, .powerups = m_powerups};
	player.tick(player_state, dt);
}

void World::draw(Shader& shader) const {
	m_background.draw(shader);
	for (auto const& enemy : m_active_enemies) { enemy->draw(shader); }
	for (auto const& emitter : m_enemy_death_emitters) { emitter.draw(shader); }
	for (auto const& powerup : m_active_powerups) { powerup->draw(shader); }
	player.draw(shader);
}

void World::on_death(Enemy const& enemy, bool const add_score) {
	if (auto source = m_resources->get<ParticleEmitter>(enemy.death_emitter)) {
		auto& emitter = m_enemy_death_emitters.emplace_back(*source);
		emitter.config.respawn = false;
		emitter.set_position(enemy.shape.transform.position);
	}

	m_audio->play_any_sfx(enemy.death_sfx);

	if (add_score) {
		m_scorer->add_score(enemy.points);
		++m_stats->player.enemies_poofed;

		// temp
		if (random_in_range(0, 10) < 3) { debug_spawn_powerup(enemy.shape.transform.position); }
		// temp
	}
}

void World::do_inspect() {
	if constexpr (bave::imgui_v) {
		if (ImGui::BeginTabItem("Player")) {
			player.inspect();
			ImGui::Separator();
			debug_controller_type();
			ImGui::EndTabItem();
		}

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

void World::debug_controller_type() {
	if constexpr (bave::imgui_v) {
		static constexpr auto type_names_v = std::array{
			PlayerController::type_name_v,
			AutoController::type_name_v,
		};

		auto const make_controller = [this](std::string_view const type_name) -> std::unique_ptr<IController> {
			if (type_name == AutoController::type_name_v) { return make_auto_controller(*this, *m_services); }
			return make_player_controller(*m_services);
		};

		auto const& controller = player.get_controller();
		if (ImGui::BeginCombo("controller", controller.get_type_name().data())) {
			for (auto const type_name : type_names_v) {
				if (ImGui::Selectable(type_name.data())) { player.set_controller(make_controller(type_name)); }
			}
			ImGui::EndCombo();
		}
	}
}

void World::debug_spawn_powerup(glm::vec2 const position) {
	auto powerup = std::make_unique<PUBeam>(*m_services);
	powerup->shape.transform.position = position;
	m_active_powerups.push_back(std::move(powerup));
}
} // namespace spaced
