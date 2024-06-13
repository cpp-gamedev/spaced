#include <bave/core/is_positive.hpp>
#include <bave/io/json_io.hpp>
#include <bave/loader.hpp>
#include <bave/persistor.hpp>
#include <spaced/prefs.hpp>
#include <spaced/scenes/load_assets.hpp>
#include <spaced/services/audio.hpp>
#include <spaced/services/gamepad_provider.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/scene_switcher.hpp>
#include <spaced/services/serializer.hpp>
#include <spaced/services/stats.hpp>
#include <spaced/services/styles.hpp>
#include <spaced/spaced.hpp>

namespace spaced {
namespace {
using bave::App;
using bave::AudioClip;
using bave::AudioDevice;
using bave::AudioStreamer;
using bave::Gamepad;
using bave::Loader;
using bave::NotNull;
using bave::Persistor;
using bave::Rect;
using bave::RenderDevice;
using bave::RenderView;
using bave::Seconds;

struct GamepadProvider : IGamepadProvider {
	bave::App& app; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

	explicit GamepadProvider(bave::App& app) : app(app) {}

	[[nodiscard]] auto get_gamepad() const -> Gamepad const& final { return app.get_most_recent_gamepad(); }
};

struct Audio : IAudio {
	NotNull<AudioDevice*> audio_device;
	NotNull<AudioStreamer*> audio_streamer;
	NotNull<Resources const*> resources;

	explicit Audio(NotNull<AudioDevice*> audio_device, NotNull<AudioStreamer*> audio_streamer, NotNull<Resources const*> resources)
		: audio_device(audio_device), audio_streamer(audio_streamer), resources(resources) {}

	[[nodiscard]] auto get_sfx_gain() const -> float final { return audio_device->sfx_gain; }
	void set_sfx_gain(float const gain) final { audio_device->sfx_gain = gain; }

	[[nodiscard]] auto get_music_gain() const -> float final { return audio_streamer->gain; }
	void set_music_gain(float const gain) final { audio_streamer->gain = gain; }

	void play_sfx(std::string_view const uri) final {
		auto const clip = resources->get<AudioClip>(uri);
		if (!clip) { return; }
		audio_device->play_once(*clip);
	}

	void play_music(std::string_view const uri, Seconds const crossfade) final {
		auto const clip = resources->get<AudioClip>(uri);
		if (!clip) { return; }
		audio_streamer->play(clip, crossfade);
	}

	void stop_music() final { audio_streamer->stop(); }
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
} // namespace

void Spaced::set_bindings([[maybe_unused]] Serializer& serializer) {}

Spaced::Spaced(App& app) : GameDriver(app) {
	m_log.info("using MSAA: {}x", static_cast<int>(app.get_render_device().get_sample_count()));
	load_resources();
	set_game_layout();
	create_services();
	set_prefs();
	set_scene();
}

void Spaced::load_resources() {
	auto const loader = Loader{&get_app().get_data_store(), &get_app().get_render_device()};
	m_resources = &m_services.get<Resources>();
	m_resources->main_font = loader.load_font("fonts/CuteDino.otf");
	m_resources->spinner = loader.load_texture("images/spinner.png", true);

	auto styles = std::make_unique<Styles>();
	if (auto const json = loader.load_json("styles.json")) {
		*styles = Styles::load(json);
		m_log.info("loaded Styles from 'styles.json'");
	}

	if constexpr (bave::debug_v) {
		static bool s_save_styles{};
		if (s_save_styles) {
			auto const json = styles->save();
			json.to_file("styles.json");
		}
	}
	m_services.bind<Styles>(std::move(styles));
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

void Spaced::set_game_layout() {
	auto game_layout = std::make_unique<GameLayout>();
	auto const& layout = m_services.get<ILayout>();
	game_layout->world_space = layout.get_world_space();
	auto const viewport = layout.get_main_view().viewport;
	auto const hud_size = glm::vec2{viewport.x, 100.0f};
	auto const hud_origin = glm::vec2{0.0f, 0.5f * (viewport.y - hud_size.y)};
	game_layout->hud_area = Rect<>::from_size(hud_size, hud_origin);
	auto const play_size = glm::vec2{hud_size.x, viewport.y - hud_size.y};
	auto const play_origin = glm::vec2{0.0f, -0.5f * (viewport.y - play_size.y)};
	game_layout->play_area = Rect<>::from_size(play_size, play_origin);
	m_services.bind<GameLayout>(std::move(game_layout));
}

void Spaced::set_prefs() {
	auto const prefs = Prefs::load(get_app());
	auto& audio = m_services.get<IAudio>();
	audio.set_music_gain(prefs.music_gain);
	audio.set_sfx_gain(prefs.sfx_gain);
}

void Spaced::set_scene() { get_switcher().switch_to<LoadAssets>(); }
} // namespace spaced
