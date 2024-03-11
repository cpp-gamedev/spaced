#include <bave/core/is_positive.hpp>
#include <bave/json_io.hpp>
#include <bave/loader.hpp>
#include <spaced/scenes/home.hpp>
#include <spaced/services/gamepad_provider.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/scene_switcher.hpp>
#include <spaced/services/serializer.hpp>
#include <spaced/services/styles.hpp>
#include <spaced/spaced.hpp>

namespace spaced {
namespace {
using bave::App;
using bave::Gamepad;
using bave::KeyInput;
using bave::Loader;
using bave::MouseScroll;
using bave::NotNull;
using bave::PointerMove;
using bave::PointerTap;
using bave::RenderDevice;
using bave::RenderView;

struct SceneSwitcherImpl : ISceneSwitcher {
	App& app;				  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
	Services const& services; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
	std::unique_ptr<Scene> next_scene{};

	explicit SceneSwitcherImpl(App& app, Services const& services) : app(app), services(services) {}

	void switch_to_scene(std::unique_ptr<Scene> new_scene) final { next_scene = std::move(new_scene); }

	[[nodiscard]] auto get_app() const -> App& final { return app; }
	[[nodiscard]] auto get_services() const -> Services const& final { return services; }
};

struct GamepadProvider : IGamepadProvider {
	bave::App& app; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

	explicit GamepadProvider(bave::App& app) : app(app) {}

	[[nodiscard]] auto get_gamepad() const -> Gamepad const& final { return app.get_most_recent_gamepad(); }
};

struct Layout : ILayout {
	NotNull<RenderDevice const*> render_device;
	RenderView main_view{};
	glm::vec2 framebuffer_size{};

	explicit Layout(NotNull<RenderDevice const*> render_device) : render_device(render_device) {}

	[[nodiscard]] auto get_main_view() const -> RenderView const& final { return main_view; }
	[[nodiscard]] auto get_framebuffer_size() const -> glm::vec2 final { return framebuffer_size; }
	[[nodiscard]] auto get_world_space() const -> glm::vec2 final { return main_view.viewport; }

	[[nodiscard]] auto project_to_world(glm::vec2 fb_point) const -> glm::vec2 final { return render_device->project_to(get_world_space(), fb_point); }

	[[nodiscard]] auto unproject(glm::vec2 const pointer) const -> glm::vec2 final {
		if (!bave::is_positive(framebuffer_size)) { return pointer; }
		return main_view.unproject(pointer / framebuffer_size);
	}

	[[nodiscard]] auto get_player_x() const -> float final { return -700.0f; }

	void set_framebuffer_size(glm::vec2 const size) final { framebuffer_size = size; }
};
} // namespace

void Spaced::set_bindings([[maybe_unused]] Serializer& serializer) {}

Spaced::Spaced(App& app) : Driver(app), m_scene(std::make_unique<Scene>(app, m_services)) {
	m_log.info("using MSAA: {}x", static_cast<int>(app.get_render_device().get_sample_count()));
	load_resources();
	set_layout();
	create_services();
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

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
	if (auto& switcher = static_cast<SceneSwitcherImpl&>(m_services.get<ISceneSwitcher>()); switcher.next_scene) {
		m_resources->clear();
		m_scene = std::move(switcher.next_scene);
	}

	m_scene->tick_frame(dt);

	clear_colour = m_scene->clear_colour;
}

void Spaced::render() const { m_scene->render_frame(); }

void Spaced::load_resources() {
	auto const loader = Loader{&get_app().get_data_store(), &get_app().get_render_device()};
	auto resources = std::make_unique<Resources>();
	resources->main_font = loader.load_font("fonts/Vera.ttf");
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
	m_services.bind<ILayout>(std::move(layout));
}

void Spaced::create_services() {
	auto serializer = std::make_unique<Serializer>();
	set_bindings(*serializer);
	m_services.bind<Serializer>(std::move(serializer));

	auto gamepad_provider = std::make_unique<GamepadProvider>(get_app());
	m_services.bind<IGamepadProvider>(std::move(gamepad_provider));
}

void Spaced::set_scene() {
	auto switcher = std::make_unique<SceneSwitcherImpl>(get_app(), m_services);
	switcher->switch_to<Home>();
	m_services.bind<ISceneSwitcher>(std::move(switcher));
}
} // namespace spaced
