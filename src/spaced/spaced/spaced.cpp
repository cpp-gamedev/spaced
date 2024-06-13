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
using bave::KeyInput;
using bave::Loader;
using bave::MouseScroll;
using bave::NotNull;
using bave::Persistor;
using bave::PointerMove;
using bave::PointerTap;
using bave::Rect;
using bave::RenderDevice;
using bave::RenderView;
using bave::Seconds;

struct GamepadProvider : IGamepadProvider {
	bave::App& app; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

	explicit GamepadProvider(bave::App& app) : app(app) {}

	[[nodiscard]] auto get_gamepad() const -> Gamepad const& final { return app.get_most_recent_gamepad(); }
};

struct Layout : ILayout {
	NotNull<RenderDevice const*> render_device;
	RenderView main_view{};
	glm::vec2 framebuffer_size{};
	Rect<> play_area{};
	Rect<> hud_area{};

	explicit Layout(NotNull<RenderDevice const*> render_device) : render_device(render_device) {}

	[[nodiscard]] auto get_main_view() const -> RenderView const& final { return main_view; }
	[[nodiscard]] auto get_framebuffer_size() const -> glm::vec2 final { return framebuffer_size; }
	[[nodiscard]] auto get_world_space() const -> glm::vec2 final { return main_view.viewport; }
	[[nodiscard]] auto get_play_area() const -> bave::Rect<> final { return play_area; }
	[[nodiscard]] auto get_hud_area() const -> bave::Rect<> final { return hud_area; }

	[[nodiscard]] auto project_to_world(glm::vec2 fb_point) const -> glm::vec2 final { return render_device->project_to(get_world_space(), fb_point); }

	[[nodiscard]] auto unproject(glm::vec2 const pointer) const -> glm::vec2 final {
		if (!bave::is_positive(framebuffer_size)) { return pointer; }
		return main_view.unproject(pointer / framebuffer_size);
	}

	[[nodiscard]] auto get_player_x() const -> float final { return -700.0f; }
	[[nodiscard]] auto get_player_size() const -> glm::vec2 final { return glm::vec2{100.0f}; }

	void set_framebuffer_size(glm::vec2 const size) final { framebuffer_size = size; }
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

struct SceneSwitcher : ISceneSwitcher {
	App& app;				  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
	Services const& services; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
	std::unique_ptr<Scene> next_scene{};

	explicit SceneSwitcher(App& app, Services const& services) : app(app), services(services) {}

	void switch_to_scene(std::unique_ptr<Scene> new_scene) final { next_scene = std::move(new_scene); }

	[[nodiscard]] auto get_app() const -> App& final { return app; }
	[[nodiscard]] auto get_services() const -> Services const& final { return services; }
};

void Spaced::set_bindings([[maybe_unused]] Serializer& serializer) {}

Spaced::Spaced(App& app) : Driver(app), m_scene(std::make_unique<EmptyScene>(app, m_services)) {
	m_log.info("using MSAA: {}x", static_cast<int>(app.get_render_device().get_sample_count()));
	load_resources();
	set_layout();
	create_services();
	set_prefs();
	set_scene();
}

void Spaced::on_focus(bave::FocusChange const& focus_change) { m_scene->on_focus_event(focus_change); }

void Spaced::on_key(KeyInput const& key_input) { m_scene->on_key_event(key_input); }

void Spaced::on_move(PointerMove const& pointer_move) { m_scene->on_move_event(pointer_move); }

void Spaced::on_tap(PointerTap const& pointer_tap) { m_scene->on_tap_event(pointer_tap); }

void Spaced::on_scroll(MouseScroll const& mouse_scroll) { m_scene->on_scroll_event(mouse_scroll); }

void Spaced::tick() {
	auto const dt = get_app().get_dt();
	m_layout->set_framebuffer_size(get_app().get_framebuffer_size());

	if (m_scene_switcher->next_scene) {
		switch_track(m_scene->get_music_uri(), m_scene_switcher->next_scene->get_music_uri());
		m_scene = std::move(m_scene_switcher->next_scene);
		m_scene->start_loading();
	}

	m_scene->tick_frame(dt);

	clear_colour = m_scene->clear_colour;
}

void Spaced::render() const { m_scene->render_frame(); }

void Spaced::load_resources() {
	auto const loader = Loader{&get_app().get_data_store(), &get_app().get_render_device()};
	auto resources = std::make_unique<Resources>();
	resources->main_font = loader.load_font("fonts/hesitation.regular.ttf");
	resources->spinner = loader.load_texture("images/spinner.png", true);
	m_resources = resources.get();
	m_services.bind<Resources>(std::move(resources));

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

void Spaced::set_layout() {
	auto const& app = get_app();
	auto layout = std::make_unique<Layout>(&app.get_render_device());
	m_layout = layout.get();
	layout->main_view = app.get_render_device().get_default_view();
	layout->main_view.viewport = app.get_render_device().get_viewport_scaler().match_height({1920.0f, 1080.0f});
	layout->framebuffer_size = app.get_framebuffer_size();
	auto const hud_size = glm::vec2{layout->main_view.viewport.x, 100.0f};
	auto const hud_origin = glm::vec2{0.0f, 0.5f * (layout->main_view.viewport.y - hud_size.y)};
	layout->hud_area = Rect<>::from_size(hud_size, hud_origin);
	auto const play_size = glm::vec2{hud_size.x, layout->main_view.viewport.y - hud_size.y};
	auto const play_origin = glm::vec2{0.0f, -0.5f * (layout->main_view.viewport.y - play_size.y)};
	layout->play_area = Rect<>::from_size(play_size, play_origin);
	m_services.bind<ILayout>(std::move(layout));
}

void Spaced::create_services() {
	auto serializer = std::make_unique<Serializer>();
	set_bindings(*serializer);
	m_services.bind<Serializer>(std::move(serializer));

	m_services.bind<IGamepadProvider>(std::make_unique<GamepadProvider>(get_app()));

	auto audio = std::make_unique<Audio>(&get_app().get_audio_device(), &get_app().get_audio_streamer(), m_resources);
	m_audio = audio.get();
	m_services.bind<IAudio>(std::move(audio));

	auto stats = std::make_unique<PersistentStats>(&get_app());
	++stats->run_count;
	m_services.bind<Stats>(std::move(stats));
}

void Spaced::set_prefs() {
	auto const prefs = Prefs::load(get_app());
	m_audio->set_music_gain(prefs.music_gain);
	m_audio->set_sfx_gain(prefs.sfx_gain);
}

void Spaced::set_scene() {
	auto switcher = std::make_unique<SceneSwitcher>(get_app(), m_services);
	m_scene_switcher = switcher.get();
	switcher->switch_to<LoadAssets>();
	m_services.bind<ISceneSwitcher>(std::move(switcher));
}

void Spaced::switch_track(std::string_view const from, std::string_view const to) const {
	if (to.empty()) {
		m_audio->stop_music();
	} else if (from != to) {
		m_audio->play_music(to);
	}
}
} // namespace spaced
