#include <spaced/scene.hpp>
#include <spaced/services/layout.hpp>
#include <algorithm>

namespace spaced {
using bave::App;
using bave::KeyInput;
using bave::PointerMove;
using bave::PointerTap;
using bave::Seconds;
using bave::Shader;

Scene::Scene(App& app, Services const& services, std::string name) : m_log{std::move(name)}, m_app(app), m_services(services) { m_log.info("constructed"); }

void Scene::on_key_event(KeyInput const& key_input) {
	if (is_loading()) { return; }
	on_key(key_input);
}

void Scene::on_move_event(PointerMove const& pointer_move) {
	if (is_loading()) { return; }
	if (on_ui_event([pointer_move](ui::View& view) { view.on_move(pointer_move); })) { return; }
	on_move(pointer_move);
}

void Scene::on_tap_event(PointerTap const& pointer_tap) {
	if (is_loading()) { return; }
	if (on_ui_event([pointer_tap](ui::View& view) { view.on_tap(pointer_tap); })) { return; }
	on_tap(pointer_tap);
}

void Scene::on_scroll_event(bave::MouseScroll const& mouse_scroll) {
	if (is_loading()) { return; }
	on_scroll(mouse_scroll);
}

auto Scene::is_ui_blocking_input() const -> bool {
	return std::any_of(m_views.begin(), m_views.end(), [](auto const& view) { return view->block_input_events; });
}

void Scene::push_view(std::unique_ptr<ui::View> view) {
	if (!view || view->is_destroyed()) { return; }
	m_views.push_back(std::move(view));
}

void Scene::tick_frame(Seconds const dt) {
	update_loading(dt);
	if (is_loading()) { return; }
	tick(dt);
	for (auto const& view : m_views) { view->tick(dt); }
	std::erase_if(m_views, [](auto const& view) { return view->is_destroyed(); });
}

void Scene::render_frame() const {
	auto shader = get_app().load_shader("shaders/default.vert", "shaders/default.frag");
	if (!shader) { return; }
	if (render_loading(*shader)) { return; }
	render(*shader);
	for (auto const& view : m_views) { view->render(*shader); }
}

void Scene::add_load_tasks(std::span<std::function<void()>> tasks) {
	if (tasks.empty()) { return; }

	m_load.emplace(tasks);
	m_loading_screen.emplace(m_services);
}

template <typename F>
auto Scene::on_ui_event(F per_view) -> bool {
	for (auto it = m_views.rbegin(); it != m_views.rend(); ++it) {
		auto const& view = *it;
		per_view(*view);
		if (view->block_input_events) { return true; }
	}
	return false;
}

void Scene::update_loading(Seconds const dt) {
	if (!m_loading_screen) { return; }

	if (!m_load) {
		m_loading_screen->update(dt, 1.0f);
		return;
	}

	auto const load_status = m_load->update();
	auto const progress = load_status.progress();
	m_loading_screen->update(dt, progress);

	if (load_status.is_complete()) {
		on_loaded();
		m_loading_screen.reset();
		m_load.reset();
	}
}

auto Scene::render_loading(Shader& shader) const -> bool {
	if (!m_loading_screen) { return false; }
	m_loading_screen->draw(shader);
	return true;
}
} // namespace spaced
