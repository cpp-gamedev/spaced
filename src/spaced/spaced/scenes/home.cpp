#include <spaced/scenes/game.hpp>
#include <spaced/scenes/home.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/scene_switcher.hpp>
#include <spaced/ui/button.hpp>
#include <spaced/ui/text.hpp>
#include <spaced/util.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::Seconds;
using bave::TextHeight;
using bave::Rgba;

Home::Home(bave::App& app, Services const& services) : Scene(app, services, "Home") {
	auto const& resources = services.get<Resources>();
	auto tasks = std::array{
		util::create_font_atlas_task(resources.main_font, {TextHeight{100}}),
	};
	add_load_tasks(tasks);
}

void Home::on_loaded() { create_ui(); }

void Home::create_ui() {

	auto m_header = std::make_unique<ui::Text>(get_services());
	m_header->text.set_height(TextHeight{100}).set_string("Home");
	m_header->text.tint = bave::white_v;

	auto start = std::make_unique<ui::Button>(get_services());
	start->set_text("start");
	start->set_position({0.0f, -200.0f});
	start->callback = [this]() { get_services().get<ISceneSwitcher>().switch_to<Game>(); };

	auto quit = std::make_unique<ui::Button>(get_services());
	quit->set_text("quit");
	quit->set_position({0.0f, -400.0f});
	quit->callback = [this]() { get_app().shutdown(); };

	auto view = std::make_unique<ui::View>(get_services());
	view->push(std::move(m_header));
	view->push(std::move(start));
	view->push(std::move(quit));
	push_view(std::move(view));
}
} // namespace spaced
