#include <bave/imgui/im_text.hpp>
#include <spaced/scenes/game.hpp>
#include <spaced/scenes/home.hpp>
#include <spaced/services/scene_switcher.hpp>

namespace spaced {
using bave::Action;
using bave::App;
using bave::im_text;
using bave::Key;
using bave::KeyInput;
using bave::KeyMods;
using bave::PointerMove;
using bave::PointerTap;
using bave::Seconds;
using bave::Shader;

Game::Game(App& app, Services const& services) : Scene(app, services, "Game"), m_player(services) {}

void Game::on_key(KeyInput const& key_input) {
	if (key_input.key == Key::eEscape && key_input.action == Action::eRelease && key_input.mods == KeyMods{}) {
		get_services().get<ISceneSwitcher>().switch_to<Home>();
	}
}

void Game::on_move(PointerMove const& pointer_move) { m_player.on_move(pointer_move); }

void Game::on_tap(PointerTap const& pointer_tap) { m_player.on_tap(pointer_tap); }

void Game::tick(Seconds const dt) {
	auto ft = bave::DeltaTime{};

	m_player.tick(dt);
	m_fps.tick(dt);

	if constexpr (bave::imgui_v) {
		if (ImGui::Begin("Debug")) {
			if (ImGui::BeginTabBar("Game")) {
				if (ImGui::BeginTabItem("Player")) {
					m_player.debug_stuff();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::Separator();
			im_text("dt: {:05.2f}", std::chrono::duration<float, std::milli>(dt).count());
			im_text("fps: {}", m_fps.fps);
			ImGui::DragInt("fps lock", &m_fps.limit);
		}
		ImGui::End();

		if (m_fps.limit > 0) {
			auto const min_dt = Seconds{1.0f / static_cast<float>(m_fps.limit)};
			auto const dt_remain = min_dt - ft.update();
			if (dt_remain > 0s) { std::this_thread::sleep_for(dt_remain); }
		}
	}
}

void Game::render(Shader& shader) const { m_player.draw(shader); }

void Game::Fps::tick(Seconds const dt) {
	elapsed += dt;
	++frames;
	if (elapsed >= 1s) {
		fps = frames;
		frames = 0;
		elapsed = 0s;
	}
}
} // namespace spaced
