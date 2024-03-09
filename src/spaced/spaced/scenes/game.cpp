#include <bave/core/random.hpp>
#include <bave/imgui/im_text.hpp>
#include <spaced/game/asset_loader.hpp>
#include <spaced/game/controllers/auto_controller.hpp>
#include <spaced/game/controllers/player_controller.hpp>
#include <spaced/game/enemies/creep.hpp>
#include <spaced/scenes/game.hpp>
#include <spaced/scenes/home.hpp>
#include <spaced/services/scene_switcher.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::Action;
using bave::App;
using bave::FixedString;
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
	return ret;
}

[[nodiscard]] auto make_auto_controller(ITargetProvider const& target_provider, Services const& services) {
	return std::make_unique<AutoController>(&target_provider, services.get<ILayout>().get_player_x());
}
} // namespace

Game::Game(App& app, Services const& services) : Scene(app, services, "Game"), m_player(services, make_player_controller(services)) {
	clear_colour = services.get<Styles>().rgbas["mocha"];
	auto asset_loader = AssetLoader{make_loader(), &services.get<Resources>()};
	auto task = asset_loader.make_load_texture("images/foam_bubble.png");
	add_load_tasks({&task, 1});
}

void Game::on_loaded() { m_player.foam_particles.emitters.front().set_texture(get_services().get<Resources>().textures["images/foam_bubble.png"]); }

void Game::on_focus(bave::FocusChange const& focus_change) { m_player.on_focus(focus_change); }

void Game::on_key(KeyInput const& key_input) {
	if (key_input.key == Key::eEscape && key_input.action == Action::eRelease && key_input.mods == KeyMods{}) {
		get_services().get<ISceneSwitcher>().switch_to<Home>();
	}
}

void Game::on_move(PointerMove const& pointer_move) { m_player.on_move(pointer_move); }

void Game::on_tap(PointerTap const& pointer_tap) { m_player.on_tap(pointer_tap); }

void Game::tick(Seconds const dt) {
	auto ft = bave::DeltaTime{};

	for (auto& enemy : m_enemies) { enemy->tick(dt); }
	std::erase_if(m_enemies, [](auto const& enemy) { return enemy->is_destroyed(); });

	m_targets.clear();
	m_targets.reserve(m_enemies.size());
	for (auto& enemy : m_enemies) { m_targets.push_back(enemy.get()); }
	m_player.tick(m_targets, dt);

	if (m_debug.next_spawn > 0s) {
		m_debug.next_spawn -= dt;
	} else {
		debug_spawn_creep();
		m_debug.next_spawn = m_debug.spawn_rate;
	}

	if constexpr (bave::debug_v) { inspect(dt, ft.update()); }
}

void Game::render(Shader& shader) const {
	for (auto const& enemy : m_enemies) { enemy->draw(shader); }
	m_player.draw(shader);
}

void Game::inspect(Seconds const dt, Seconds const frame_time) {
	if constexpr (bave::imgui_v) {
		m_debug.fps.tick(dt);

		if (ImGui::Begin("Debug")) {
			if (ImGui::BeginTabBar("Game")) {
				if (ImGui::BeginTabItem("Player")) {
					m_player.inspect();
					ImGui::Separator();
					debug_controller_type();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Enemies")) {
					debug_inspect_enemies();
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

void Game::debug_inspect_enemies() {
	if constexpr (bave::imgui_v) {
		im_text("creeps: {}", m_enemies.size());
		if (ImGui::Button("spawn creep")) { debug_spawn_creep(); }
		auto spawn_rate = m_debug.spawn_rate.count();
		if (ImGui::DragFloat("spawn rate", &spawn_rate, 0.25f, 0.25f, 10.0f)) { m_debug.spawn_rate = Seconds{spawn_rate}; }

		ImGui::Separator();
		for (std::size_t i = 0; i < m_enemies.size(); ++i) {
			if (ImGui::TreeNode(FixedString{"{}", i}.c_str())) {
				m_enemies.at(i)->inspect();
				ImGui::TreePop();
			}
		}
	}
}

void Game::debug_spawn_creep() {
	auto creep = std::make_unique<Creep>(get_services(), this);
	creep->shape.tint = bave::yellow_v;
	m_enemies.push_back(std::move(creep));
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

		auto const& controller = m_player.get_controller();
		if (ImGui::BeginCombo("controller", controller.get_type_name().data())) {
			for (auto const type_name : type_names_v) {
				if (ImGui::Selectable(type_name.data())) { m_player.set_controller(make_controller(type_name)); }
			}
			ImGui::EndCombo();
		}
	}
}
} // namespace spaced
