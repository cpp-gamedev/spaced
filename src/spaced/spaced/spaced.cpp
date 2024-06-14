#include <bave/core/is_positive.hpp>
#include <bave/io/json_io.hpp>
#include <bave/loader.hpp>
#include <bave/persistor.hpp>
#include <bave/services/audio.hpp>
#include <bave/services/resources.hpp>
#include <bave/services/styles.hpp>
#include <spaced/prefs.hpp>
#include <spaced/scenes/load_assets.hpp>
#include <spaced/services/gamepad_provider.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/serializer.hpp>
#include <spaced/services/stats.hpp>
#include <spaced/spaced.hpp>
#include <array>

namespace spaced {
namespace {
using bave::App;
using bave::GameDriver;
using bave::Gamepad;
using bave::IAudio;
using bave::IDisplay;
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
	static constexpr std::string_view uri_v{"spaced/stats.json"};

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

constexpr auto preload_text_heights_v = std::array{TextHeight{100}, TextHeight{60}};

constexpr auto gdci_v = bave::GameDriver::CreateInfo{
	.assets =
		{
			.main_font =
				{
					.uri = "fonts/CuteDino.otf",
					.preload_heights = preload_text_heights_v,
				},
			.spinner = "images/spinner.png",
			.styles = "styles.json",
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
}

void Spaced::set_layout() {
	auto game_layout = std::make_unique<Layout>();
	auto const& display = m_services.get<IDisplay>();
	game_layout->world_space = display.get_world_space();
	auto const viewport = display.get_main_view().viewport;
	auto const hud_size = glm::vec2{viewport.x, 100.0f};
	auto const hud_origin = glm::vec2{0.0f, 0.5f * (viewport.y - hud_size.y)};
	game_layout->hud_area = Rect<>::from_size(hud_size, hud_origin);
	auto const play_size = glm::vec2{hud_size.x, viewport.y - hud_size.y};
	auto const play_origin = glm::vec2{0.0f, -0.5f * (viewport.y - play_size.y)};
	game_layout->play_area = Rect<>::from_size(play_size, play_origin);
	m_services.bind<Layout>(std::move(game_layout));
}

void Spaced::set_prefs() {
	auto const prefs = Prefs::load(get_app());
	auto& audio = m_services.get<IAudio>();
	audio.set_music_gain(prefs.music_gain);
	audio.set_sfx_gain(prefs.sfx_gain);
}

void Spaced::set_scene() { get_switcher().switch_to<LoadAssets>(); }
} // namespace spaced
