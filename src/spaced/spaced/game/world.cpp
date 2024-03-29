#include <bave/imgui/im_text.hpp>
#include <spaced/game/enemy_factory_builder.hpp>
#include <spaced/game/world.hpp>

#include <bave/core/random.hpp>
#include <spaced/game/controllers/auto_controller.hpp>
#include <spaced/game/controllers/player_controller.hpp>
#include <spaced/game/powerups/pu_beam.hpp>

namespace spaced {
using bave::FixedString;
using bave::NotNull;
using bave::random_in_range;
using bave::Seconds;
using bave::Shader;

namespace {
[[nodiscard]] auto make_player_controller(Services const& services) {
	auto ret = std::make_unique<PlayerController>(services);
	if constexpr (bave::platform_v == bave::Platform::eAndroid) { ret->set_type(PlayerController::Type::eTouch); }
	auto const& layout = services.get<ILayout>();
	auto const half_size = 0.5f * layout.get_player_size();
	auto const play_area = layout.get_play_area();
	ret->max_y = play_area.lt.y - half_size.y;
	ret->min_y = play_area.rb.y + half_size.y;
	return ret;
}

[[nodiscard]] auto make_auto_controller(ITargetProvider const& target_provider, Services const& services) {
	return std::make_unique<AutoController>(&target_provider, services.get<ILayout>().get_player_x());
}
} // namespace

World::World(bave::NotNull<Services const*> services, bave::NotNull<IScorer*> scorer)
	: player(*services, make_player_controller(*services)), m_services(services), m_scorer(scorer) {}

void World::on_death(EnemyDeath const& death) {
	m_scorer->add_score(death.points);

	// temp
	if (random_in_range(0, 10) < 3) { debug_spawn_powerup(death.position); }
	// temp
}

void World::tick(Seconds const dt) {
	m_targets.clear();
	for (auto& spawner : m_enemy_spawners) {
		spawner.tick(dt);
		spawner.append_targets(m_targets);
	}

	for (auto const& powerup : m_powerups) { powerup->tick(dt); }
	std::erase_if(m_powerups, [](auto const& powerup) { return powerup->is_destroyed(); });

	m_powerups_view.clear();
	for (auto const& powerup : m_powerups) { m_powerups_view.emplace_back(powerup.get()); }

	auto const player_state = Player::State{.targets = m_targets, .powerups = m_powerups_view};
	player.tick(player_state, dt);
}

void World::draw(Shader& shader) const {
	for (auto const& spawner : m_enemy_spawners) { spawner.draw(shader); }
	for (auto const& powerup : m_powerups) { powerup->draw(shader); }
	player.draw(shader);
}

void World::load(WorldSpec const& spec) {
	m_enemy_spawners.clear();
	auto const factory = EnemyFactoryBuilder{m_services, this};
	for (auto const& factory_json : spec.enemy_factories) { m_enemy_spawners.emplace_back(factory.build(factory_json)); }

	player.setup(spec.player);
}

void World::do_inspect() {
	if constexpr (bave::imgui_v) {
		if (ImGui::BeginTabItem("Player")) {
			player.inspect();
			ImGui::Separator();
			debug_controller_type();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Enemy Spawners")) {
			inspect_enemy_spawners();
			ImGui::EndTabItem();
		}
	}
}

void World::inspect_enemy_spawners() {
	if constexpr (bave::imgui_v) {
		for (std::size_t i = 0; i < m_enemy_spawners.size(); ++i) {
			if (ImGui::TreeNode(FixedString{"[{}]", i}.c_str())) {
				m_enemy_spawners.at(i).inspect();
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
	m_powerups.push_back(std::move(powerup));
}
} // namespace spaced
