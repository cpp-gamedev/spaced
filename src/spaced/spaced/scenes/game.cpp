#include <bave/core/random.hpp>
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
using bave::Ptr;
using bave::Seconds;
using bave::Shader;

Game::Game(App& app, Services const& services) : Scene(app, services, "Game"), m_player(services) {}

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

	m_player.tick(m_targets, dt);

	if constexpr (bave::debug_v) { inspect(dt, ft.update()); }
}

void Game::render(Shader& shader) const { m_player.draw(shader); }

void Game::inspect(Seconds const dt, Seconds const frame_time) {
	if constexpr (bave::imgui_v) {
		m_debug.fps.tick(dt);

		if (ImGui::Begin("Debug")) {
			if (ImGui::BeginTabBar("Game")) {
				if (ImGui::BeginTabItem("Player")) {
					m_player.inspect();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

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
} // namespace spaced
