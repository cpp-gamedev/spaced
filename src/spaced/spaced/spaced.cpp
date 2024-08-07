#include <bave/core/is_positive.hpp>
#include <bave/io/json_io.hpp>
#include <bave/loader.hpp>
#include <bave/persistor.hpp>
#include <bave/services/audio.hpp>
#include <bave/services/resources.hpp>
#include <bave/services/styles.hpp>
#include <spaced/prefs.hpp>
#include <spaced/scenes/menu.hpp>
#include <spaced/services/game_signals.hpp>
#include <spaced/services/gamepad_provider.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/serializer.hpp>
#include <spaced/services/stats.hpp>
#include <spaced/spaced.hpp>
#include <array>

namespace spaced {
namespace {
using bave::App;
using bave::Display;
using bave::GameDriver;
using bave::Gamepad;
using bave::IAudio;
using bave::NotNull;
using bave::Persistor;
using bave::Rect;
using bave::Seconds;
using bave::Styles;
using bave::TextHeight;

struct GamepadProvider : IGamepadProvider {
	bave::App& app; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

	explicit GamepadProvider(bave::App& app) : app(app) {}

	[[nodiscard]] auto get_gamepad() const -> Gamepad const& final { return app.get_most_recent_gamepad(); }
};

struct PersistentStats : Stats {
	static constexpr std::string_view uri_v{"userdata/stats.json"};

	NotNull<App const*> app;

	PersistentStats(PersistentStats const&) = delete;
	PersistentStats(PersistentStats&&) = delete;
	auto operator=(PersistentStats const&) = delete;
	auto operator=(PersistentStats&&) = delete;

	PersistentStats(NotNull<App const*> app) : app(app) {
		auto const persistor = Persistor{*app};
		if (!persistor.exists(uri_v)) { return; }
		auto const json = persistor.read_json(uri_v);
		from(json);
	}

	~PersistentStats() override {
		auto json = dj::Json{};
		to(json);
		auto const persistor = Persistor{*app};
		persistor.write_json(uri_v, json);
	}
};

constexpr auto gdci_v = bave::GameDriver::CreateInfo{
	.assets =
		{
			.main_font =
				{
					.uri = "assets/fonts/CuteDino.otf",
				},
			.spinner = "assets/images/spinner.png",
			.styles = "assets/styles.json",
			.interact_sfx = "assets/sfx/interact.wav",
		},
};
} // namespace

void Spaced::set_bindings([[maybe_unused]] Serializer& serializer) {}

Spaced::Spaced(App& app) : GameDriver(app, gdci_v) {
	m_log.info("using MSAA: {}x", static_cast<int>(app.get_render_device().get_sample_count()));
	save_styles();
	set_layout();
	create_services();
	set_prefs();
	set_scene();

	if constexpr (bave::imgui_v && !bave::debug_v) { ImGui::GetIO().IniFilename = ""; }
}

void Spaced::save_styles() {
	if constexpr (bave::debug_v) {
		static bool s_save_styles{};
		if (s_save_styles) {
			auto const json = m_services.get<Styles>().save();
			json.to_file("styles.json");
		}
	}
}

void Spaced::create_services() {
	auto serializer = std::make_unique<Serializer>();
	set_bindings(*serializer);
	m_services.bind<Serializer>(std::move(serializer));

	m_services.bind<IGamepadProvider>(std::make_unique<GamepadProvider>(get_app()));

	auto stats = std::make_unique<PersistentStats>(&get_app());
	++stats->run_count;
	m_services.bind<Stats>(std::move(stats));

	m_services.bind<GameSignals>(std::make_unique<GameSignals>());
}

void Spaced::set_layout() {
	static constexpr auto world_space_v = glm::vec2{1920.0f, 1080.0f};

	auto layout = std::make_unique<Layout>(&get_app().get_render_device());
	auto& world_space = m_services.get<Display>().world;
	world_space.render_view.viewport = world_space.get_viewport_scaler().match_width(world_space_v);

	layout->world_space = world_space.get_size();
	layout->player_x = -0.5f * layout->world_space.x + 0.2f * layout->world_space.x;
	auto const hud_size = glm::vec2{layout->world_space.x, 100.0f};
	auto const hud_origin = glm::vec2{0.0f, 0.5f * (layout->world_space.y - hud_size.y)};
	layout->hud_area = Rect<>::from_size(hud_size, hud_origin);
	auto const play_size = glm::vec2{hud_size.x, layout->world_space.y - hud_size.y};
	auto const play_origin = glm::vec2{0.0f, -0.5f * (layout->world_space.y - play_size.y)};
	layout->play_area = Rect<>::from_size(play_size, play_origin);
	m_services.bind<Layout>(std::move(layout));
}

void Spaced::set_prefs() {
	auto const prefs = Prefs::load(get_app());
	auto& audio = m_services.get<IAudio>();
	audio.set_music_gain(prefs.music_gain);
	audio.set_sfx_gain(prefs.sfx_gain);
}

void Spaced::set_scene() { get_switcher().switch_to<MenuScene>(); }
} // namespace spaced
