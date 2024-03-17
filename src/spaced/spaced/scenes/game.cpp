#include <bave/core/random.hpp>
#include <bave/imgui/im_text.hpp>
#include <spaced/game/asset_list.hpp>
#include <spaced/game/controllers/auto_controller.hpp>
#include <spaced/game/controllers/player_controller.hpp>
#include <spaced/game/enemy_factory_builder.hpp>
#include <spaced/scenes/game.hpp>
#include <spaced/scenes/home.hpp>
#include <spaced/services/scene_switcher.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::Action;
using bave::App;
using bave::FixedString;
using bave::FocusChange;
using bave::im_text;
using bave::Key;
using bave::KeyInput;
using bave::KeyMods;
using bave::PointerMove;
using bave::PointerTap;
using bave::Ptr;
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

Game::Game(App& app, Services const& services) : Scene(app, services, "Game") {
	auto asset_list = AssetList{make_loader(), get_services()};
	m_world = asset_list.read_world("worlds/playground.json");
	clear_colour = services.get<Styles>().rgbas[m_world.background_tint];
	add_load_stages(asset_list.build_task_stages());
}

void Game::on_loaded() {
	m_player.emplace(get_services(), make_player_controller(get_services()));

	auto const factory = EnemyFactoryBuilder{&get_services(), this};
	for (auto const& factory_json : m_world.enemy_factories) { m_enemy_spawners.emplace_back(factory.build(factory_json)); }

	auto hud = std::make_unique<Hud>(get_services());
	m_hud = hud.get();
	push_view(std::move(hud));
}

void Game::on_focus(FocusChange const& focus_change) { m_player->on_focus(focus_change); }

void Game::on_key(KeyInput const& key_input) {
	if (key_input.key == Key::eEscape && key_input.action == Action::eRelease && key_input.mods == KeyMods{}) {
		get_services().get<ISceneSwitcher>().switch_to<Home>();
	}
}

void Game::on_move(PointerMove const& pointer_move) { m_player->on_move(pointer_move); }

void Game::on_tap(PointerTap const& pointer_tap) { m_player->on_tap(pointer_tap); }

void Game::tick(Seconds const dt) {
	auto ft = bave::DeltaTime{};

	m_targets.clear();
	for (auto& spawner : m_enemy_spawners) {
		spawner.tick(dt);
		spawner.append_targets(m_targets);
	}

	m_player->tick(m_targets, dt);

	if constexpr (bave::debug_v) { inspect(dt, ft.update()); }
}

void Game::render(Shader& shader) const {
	for (auto const& spawner : m_enemy_spawners) { spawner.draw(shader); }
	m_player->draw(shader);
}

void Game::add_score(std::int64_t const score) {
	m_score += score;
	m_hud->set_score(m_score);
}

void Game::inspect(Seconds const dt, Seconds const frame_time) {
	if constexpr (bave::imgui_v) {
		m_debug.fps.tick(dt);

		if (ImGui::Begin("Debug")) {
			if (ImGui::BeginTabBar("Game")) {
				if (ImGui::BeginTabItem("Player")) {
					m_player->inspect();
					ImGui::Separator();
					debug_controller_type();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Enemy Spawners")) {
					inspect_enemy_spawners();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::Separator();
			im_text("score: {}", get_score());

			ImGui::Separator();
			im_text("dt: {:05.2f}", std::chrono::duration<float, std::milli>(dt).count());
			im_text("fps: {}", m_debug.fps.fps);
			ImGui::SliderInt("fps limit", &m_debug.fps.limit, 5, 1000);
			ImGui::Checkbox("fps lock", &m_debug.fps.lock);
		}
		ImGui::End();

		if (m_debug.fps.lock) {
			m_debug.fps.limit = std::clamp(m_debug.fps.limit, 5, 1000);
			auto const min_dt = Seconds{1.0f / static_cast<float>(m_debug.fps.limit)};
			auto const dt_remain = min_dt - frame_time;
			if (dt_remain > 0s) { std::this_thread::sleep_for(dt_remain); }
		}
	}
}

void Game::inspect_enemy_spawners() {
	if constexpr (bave::imgui_v) {
		for (std::size_t i = 0; i < m_enemy_spawners.size(); ++i) {
			if (ImGui::TreeNode(FixedString{"[{}]", i}.c_str())) {
				m_enemy_spawners.at(i).inspect();
				ImGui::TreePop();
			}
		}
	}
}

void Game::debug_controller_type() {
	if constexpr (bave::imgui_v) {
		static constexpr auto type_names_v = std::array{
			PlayerController::type_name_v,
			AutoController::type_name_v,
		};

		auto const make_controller = [this](std::string_view const type_name) -> std::unique_ptr<IController> {
			if (type_name == AutoController::type_name_v) { return make_auto_controller(*this, get_services()); }
			return make_player_controller(get_services());
		};

		auto const& controller = m_player->get_controller();
		if (ImGui::BeginCombo("controller", controller.get_type_name().data())) {
			for (auto const type_name : type_names_v) {
				if (ImGui::Selectable(type_name.data())) { m_player->set_controller(make_controller(type_name)); }
			}
			ImGui::EndCombo();
		}
	}
}
} // namespace spaced
