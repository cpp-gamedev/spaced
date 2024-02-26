#include <bave/core/is_positive.hpp>
#include <bave/json_io.hpp>
#include <bave/loader.hpp>
#include <spaced/scenes/home.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/scene_switcher.hpp>
#include <spaced/services/serializer.hpp>
#include <spaced/services/styles.hpp>
#include <spaced/spaced.hpp>

using bave::App;
using bave::KeyInput;
using bave::Loader;
using bave::MouseScroll;
using bave::PointerMove;
using bave::PointerTap;

namespace spaced {
namespace {
using bave::AnimTimeline;
using bave::NotNull;
using bave::RenderDevice;
using bave::RenderView;
using bave::Texture;
using bave::TextureAtlas;

struct SceneSwitcherImpl : ISceneSwitcher {
	App& app;				  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
	Services const& services; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
	std::unique_ptr<Scene> next_scene{};

	explicit SceneSwitcherImpl(App& app, Services const& services) : app(app), services(services) {}

	void switch_to(std::unique_ptr<Scene> new_scene) final { next_scene = std::move(new_scene); }
};

struct TestScene : Scene {
	TestScene(bave::App& app, Services const& services) : Scene(app, services) {
		using F = std::function<void()>;
		auto tasks = std::array{
			F{[] { std::this_thread::sleep_for(1s); }},
			F{[] { std::this_thread::sleep_for(0.5s); }},
			F{[] { std::this_thread::sleep_for(0.3s); }},
		};
		add_load_tasks(tasks);
	}
};

// struct LevelLoaderImpl : ILevelLoader {
// 	explicit LevelLoaderImpl(bave::App const& app, bave::NotNull<Serializer const*> serializer)
// 		: m_serializer(serializer), m_loader{&app.get_data_store(), &app.get_render_device()} {}

//   private:
// 	template <typename Type>
// 	using Map = std::unordered_map<std::string, Type, bave::StringHash, std::equal_to<>>;

// 	[[nodiscard]] auto load_level(dj::Json const& json) -> Result final {
// 		auto ret = Result{};
// 		auto map = load_data_map(json["map"]);
// 		auto const list = load_instances(json["list"]);
// 		if (auto const& shared = json["shared"]) {
// 			auto destroy_animation = std::unique_ptr<ItemAnimation>{};
// 			if (auto const& in_destroy_anim = shared["destroy_animation"]) { destroy_animation = m_serializer->deserialize_as<ItemAnimation>(in_destroy_anim); }

// 			for (auto& [_, item] : map) {
// 				if (item.is_collectible && !item.destroy_animation) { item.destroy_animation = destroy_animation->clone(); }
// 			}
// 		}
// 		for (auto const& item : list) {
// 			auto const it = map.find(item.name);
// 			if (it == map.end()) {
// 				m_log.error("item name not in map: '{}'", item.name);
// 				continue;
// 			}
// 			auto& entity = ret.entities.emplace_back();
// 			entity.set_item(it->second, item.position);
// 		}
// 		for (auto& [_, item] : map) { ret.items.push_back(std::move(item)); }
// 		return ret;
// 	}

// 	[[nodiscard]] auto load_data_map(dj::Json const& json) -> Map<ItemData> {
// 		auto ret = Map<ItemData>{};
// 		for (auto const& item : json.array_view()) { add_item_data(ret, item); }
// 		return ret;
// 	}

// 	[[nodiscard]] auto load_instances(dj::Json const& json) const -> std::vector<ItemInstance> {
// 		using bave::from_json;
// 		auto ret = std::vector<ItemInstance>{};
// 		for (auto const& in_item : json.array_view()) {
// 			auto const name = in_item["name"].as_string();
// 			if (name.empty()) {
// 				m_log.error("empty item name");
// 				continue;
// 			}
// 			auto item = ItemInstance{.name = std::string{name}};
// 			from_json(in_item["position"], item.position);
// 			ret.push_back(std::move(item));
// 		}
// 		return ret;
// 	}

// 	void add_item_data(Map<ItemData>& out, dj::Json const& json) {
// 		auto name = json["name"].as_string();
// 		if (name.empty()) { return; }
// 		auto item = ItemData{};
// 		item.name = name;
// 		item.texture.texture = load_texture(json["texture"].as_string());
// 		item.texture.atlas = load_texture_atlas(json["atlas"].as_string());
// 		item.texture.timeline = load_anim_timeline(json["timeline"].as_string());
// 		for (auto const& in_animation : json["animations"].array_view()) {
// 			if (auto out_animation = m_serializer->deserialize_as<ItemAnimation>(in_animation)) { item.animations.push_back(std::move(out_animation)); }
// 		}
// 		if (auto const& destroy_animation = json["destroy_animation"]) {
// 			item.destroy_animation = m_serializer->deserialize_as<ItemAnimation>(destroy_animation);
// 		}
// 		item.idle_tile = json["idle_tile"].as_string();
// 		item.hitbox_diameter = json["hitbox_diameter"].as<float>(item.hitbox_diameter);
// 		item.is_collectible = json["is_collectible"].as<bool>(item.is_collectible);
// 		out.insert_or_assign(std::string{name}, std::move(item));
// 	}

// 	[[nodiscard]] auto load_texture(std::string_view uri) -> std::shared_ptr<bave::Texture> {
// 		if (uri.empty()) { return {}; }
// 		if (auto const it = m_textures.find(uri); it != m_textures.end()) { return it->second; }

// 		auto ret = m_loader.load_texture(uri);
// 		if (ret) { m_textures.insert_or_assign(std::string{uri}, ret); }
// 		return ret;
// 	}

// 	[[nodiscard]] auto load_texture_atlas(std::string_view uri) -> std::shared_ptr<bave::TextureAtlas> {
// 		if (uri.empty()) { return {}; }
// 		if (auto const it = m_textures.find(uri); it != m_textures.end()) { return std::dynamic_pointer_cast<TextureAtlas>(it->second); }

// 		auto ret = m_loader.load_texture_atlas(uri);
// 		if (ret) { m_textures.insert_or_assign(std::string{uri}, ret); }
// 		return ret;
// 	}

// 	[[nodiscard]] auto load_anim_timeline(std::string_view uri) -> std::shared_ptr<bave::AnimTimeline> {
// 		if (uri.empty()) { return {}; }
// 		if (auto const it = m_timelines.find(uri); it != m_timelines.end()) { return it->second; }

// 		auto ret = m_loader.load_anim_timeline(uri);
// 		if (ret) { m_timelines.insert_or_assign(std::string{uri}, ret); }
// 		return ret;
// 	}

// 	bave::Logger m_log{"LevelLoader"};
// 	bave::NotNull<Serializer const*> m_serializer;
// 	bave::Loader m_loader;

// 	Map<std::shared_ptr<bave::Texture>> m_textures{};
// 	Map<std::shared_ptr<bave::AnimTimeline>> m_timelines{};
// };
} // namespace

struct Spaced::Layout : ILayout {
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
};

void Spaced::set_bindings(Serializer& serializer) {
	// serializer.bind<Oscillator>();
	// serializer.bind<Shrinker>();
}

Spaced::Spaced(App& app) : Driver(app), m_scene(std::make_unique<Scene>(app, m_services)) {
	m_log.info("using MSAA: {}x", static_cast<int>(app.get_render_device().get_sample_count()));
	load_resources();
	set_layout();
	create_services();
	set_scene();
}

void Spaced::on_key(KeyInput const& key_input) { m_scene->on_key_event(key_input); }

void Spaced::on_move(PointerMove const& pointer_move) { m_scene->on_move_event(pointer_move); }

void Spaced::on_tap(PointerTap const& pointer_tap) { m_scene->on_tap_event(pointer_tap); }

void Spaced::on_scroll(MouseScroll const& mouse_scroll) { m_scene->on_scroll_event(mouse_scroll); }

void Spaced::tick() {
	auto const dt = get_app().get_dt();
	m_layout->framebuffer_size = get_app().get_framebuffer_size();

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast)
	if (auto& switcher = static_cast<SceneSwitcherImpl&>(m_services.get<ISceneSwitcher>()); switcher.next_scene) { m_scene = std::move(switcher.next_scene); }

	m_scene->tick_frame(dt);

	clear_colour = m_scene->clear_colour;
}

void Spaced::render() const { m_scene->render_frame(); }

void Spaced::load_resources() {
	auto const loader = Loader{&get_app().get_data_store(), &get_app().get_render_device()};
	auto resources = std::make_unique<Resources>();
	resources->main_font = loader.load_font("fonts/Vera.ttf");
	resources->spinner = loader.load_texture("images/spinner.png", true);
	m_services.bind<Resources>(std::move(resources));

	auto styles = std::make_unique<ui::Styles>();
	if (auto const json = loader.load_json("styles.json")) {
		*styles = ui::Styles::load(json);
		m_log.info("loaded Styles from 'styles.json'");
	}

	if constexpr (bave::debug_v) {
		static bool s_save_styles{};
		if (s_save_styles) {
			auto const json = styles->save();
			json.to_file("styles.json");
		}
	}
	m_services.bind<ui::Styles>(std::move(styles));
}

void Spaced::set_layout() {
	auto const& app = get_app();
	auto layout = std::make_unique<Layout>(&app.get_render_device());
	m_layout = layout.get();
	layout->main_view = app.get_render_device().get_default_view();
	layout->main_view.viewport = app.get_render_device().get_viewport_scaler().match_width({1920.0f, 1080.0f});
	layout->framebuffer_size = app.get_framebuffer_size();
	m_services.bind<ILayout>(std::move(layout));
}

void Spaced::create_services() {
	auto serializer = std::make_unique<Serializer>();
	set_bindings(*serializer);
	auto const* p_serializer = serializer.get();
	m_services.bind<Serializer>(std::move(serializer));

	// auto level_loader = std::make_unique<LevelLoaderImpl>(get_app(), p_serializer);
	// m_services.bind<ILevelLoader>(std::move(level_loader));
}

void Spaced::set_scene() {
	auto switcher = std::make_unique<SceneSwitcherImpl>(get_app(), m_services);
	auto* p_switcher = switcher.get();
	m_services.bind<ISceneSwitcher>(std::move(switcher));
	// p_switcher->switch_to_home();
	// m_scene = std::move(p_switcher->next_scene);

	p_switcher->switch_to(std::make_unique<Home>(get_app(), m_services));
}
} // namespace spaced
