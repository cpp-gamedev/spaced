#include <bave/assets/asset_list.hpp>
#include <bave/assets/asset_manifest.hpp>
#include <bave/core/random.hpp>
#include <bave/imgui/im_text.hpp>
#include <bave/services/scene_switcher.hpp>
#include <bave/services/styles.hpp>
#include <bave/ui/button.hpp>
#include <bave/ui/dialog.hpp>
#include <spaced/game/controllers/auto_controller.hpp>
#include <spaced/game/controllers/player_controller.hpp>
#include <spaced/scenes/game.hpp>
#include <spaced/scenes/menu.hpp>
#include <spaced/services/stats.hpp>

namespace spaced {
using bave::Action;
using bave::App;
using bave::AssetList;
using bave::AssetManifest;
using bave::AsyncExec;
using bave::FocusChange;
using bave::im_text;
using bave::Key;
using bave::KeyInput;
using bave::KeyMods;
using bave::PointerMove;
using bave::PointerTap;
using bave::Ptr;
using bave::Seconds;
using bave::Services;
using bave::Shader;
using bave::Styles;

namespace ui = bave::ui;

namespace {
auto get_manifest() -> AssetManifest {
	return AssetManifest{
		.textures =
			{
				"images/player_ship.png",
				"images/shield.png",
				"images/creep_ship.png",
				"images/background.png",
				"images/kinetic_projectile.png",
			},
		.audio_clips =
			{
				"sfx/swish.wav",
				"sfx/kinetic_fire.wav",
				"sfx/beam_fire.wav",
				"music/game.mp3",
			},
		.particle_emitters =
			{
				"particles/exhaust.json",
				"particles/explode.json",
				"particles/powerup.json",
			},
	};
}

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

GameScene::GameScene(App& app, Services const& services) : Scene(app, services, "Game"), m_save(&app) { clear_colour = services.get<Styles>().rgbas["mocha"]; }

auto GameScene::build_load_stages() -> std::vector<AsyncExec::Stage> {
	auto ret = std::vector<AsyncExec::Stage>{};
	auto asset_list = AssetList{make_loader(), get_services()};
	asset_list.add_manifest(get_manifest());
	ret.push_back(asset_list.build_load_stage());
	return ret;
}

void GameScene::on_loaded() {
	auto const& services = get_services();
	m_world.emplace(&services, this);

	m_player.emplace(services, make_player_controller(services));

	auto hud = std::make_unique<Hud>(services);
	m_hud = hud.get();
	m_hud->set_hi_score(m_save.get_hi_score());
	push_view(std::move(hud));

	++services.get<Stats>().game.play_count;

	switch_track("music/game.mp3");
}

void GameScene::on_focus(FocusChange const& focus_change) { m_player->on_focus(focus_change); }

void GameScene::on_key(KeyInput const& key_input) {
	if (key_input.key == Key::eEscape && key_input.action == Action::eRelease && key_input.mods == KeyMods{}) { get_switcher().switch_to<MenuScene>(); }
}

void GameScene::on_move(PointerMove const& pointer_move) { m_player->on_move(pointer_move); }

void GameScene::on_tap(PointerTap const& pointer_tap) { m_player->on_tap(pointer_tap); }

void GameScene::tick(Seconds const dt) {
	auto ft = bave::DeltaTime{};

	auto const game_over = m_player->is_idle() && m_spare_lives == 0;
	m_world->tick(dt, !game_over);

	auto const player_state = Player::State{.targets = m_world->get_targets(), .powerups = m_world->get_powerups()};
	m_player->tick(player_state, dt);

	if (m_player->is_idle()) { on_player_death(); }

	if constexpr (bave::debug_v) { inspect(dt, ft.update()); }
}

void GameScene::render(Shader& shader) const {
	m_world->draw(shader);
	m_player->draw(shader);
}

void GameScene::add_score(std::int64_t const score) {
	m_score += score;
	m_hud->set_score(m_score);
	update_hi_score();
}

void GameScene::on_player_death() {
	if (m_spare_lives > 0) {
		--m_spare_lives;
		respawn_player();
		return;
	}

	if (!m_game_over_dialog_pushed) {
		m_game_over_dialog_pushed = true;
		on_game_over();
	}
}

void GameScene::respawn_player() {
	m_player.emplace(get_services(), make_player_controller(get_services()));
	m_player->set_shield(2s);
}

void GameScene::on_game_over() {
	auto dci = ui::DialogCreateInfo{
		.size = {600.0f, 200.0f},
		.content_text = "GAME OVER",
		.main_button = {.text = "RESTART", .callback = [this] { get_switcher().switch_to<GameScene>(); }},
		.second_button = {.text = "QUIT", .callback = [this] { get_app().shutdown(); }},
	};

	auto dialog = std::make_unique<ui::Dialog>(get_services(), std::move(dci));
	push_view(std::move(dialog));
}

void GameScene::update_hi_score() {
	if (m_score <= m_save.get_hi_score()) { return; }
	m_save.set_hi_score(m_score);
	m_hud->set_hi_score(m_save.get_hi_score());
}

void GameScene::inspect(Seconds const dt, Seconds const frame_time) {
	if constexpr (bave::imgui_v) {
		m_debug.fps.tick(dt);

		if (ImGui::Begin("Debug")) {
			if (ImGui::BeginTabBar("World")) {
				if (ImGui::BeginTabItem("Player")) {
					m_player->inspect();
					ImGui::Separator();
					debug_controller_type();
					ImGui::EndTabItem();
				}

				m_world->inspect();
				ImGui::EndTabBar();
			}

			ImGui::Separator();
			im_text("score: {}", get_score());

			ImGui::Separator();
			if (ImGui::Button("end game")) { m_player->on_death(dt); }

			ImGui::Separator();
			im_text("dt: {:05.2f}", std::chrono::duration<float, std::milli>(dt).count());
			im_text("fps: {}", m_debug.fps.fps);
			ImGui::SliderInt("fps limit", &m_debug.fps.limit, 5, 1000);
			ImGui::Checkbox("fps lock", &m_debug.fps.lock);

			ImGui::Separator();
			if (ImGui::Button("reload scene")) { get_switcher().switch_to<GameScene>(); }
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

void GameScene::debug_controller_type() {
	if constexpr (bave::imgui_v) {
		static constexpr auto type_names_v = std::array{
			PlayerController::type_name_v,
			AutoController::type_name_v,
		};

		auto const make_controller = [this](std::string_view const type_name) -> std::unique_ptr<IController> {
			if (type_name == AutoController::type_name_v) { return make_auto_controller(*m_world, get_services()); }
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
