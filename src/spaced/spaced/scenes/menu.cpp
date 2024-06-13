#include <spaced/prefs.hpp>
#include <spaced/scenes/game.hpp>
#include <spaced/scenes/menu.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/scene_switcher.hpp>
#include <spaced/ui/button.hpp>
#include <spaced/ui/dialog.hpp>
#include <spaced/ui/text.hpp>
#include <spaced/util.hpp>

namespace spaced {
using bave::App;
using bave::Seconds;
using bave::TextHeight;

auto MenuScene::get_text_heights() -> std::vector<TextHeight> { return {TextHeight{100}, TextHeight{60}, ui::Dialog::text_height_v}; }

MenuScene::MenuScene(App& app, Services const& services) : Scene(app, services, "Home") { create_ui(); }

void MenuScene::create_ui() {
	auto m_header = std::make_unique<ui::Text>(get_services());
	m_header->text.set_height(TextHeight{100}).set_string("Home");
	m_header->set_position({0.0f, 100.0f});
	m_header->text.tint = bave::white_v;

	auto start = std::make_unique<ui::Button>(get_services());
	start->set_text("start");
	start->set_position({0.0f, -100.0f});
	start->callback = [this]() { get_switcher().switch_to<GameScene>(); };

	auto options = std::make_unique<ui::Button>(get_services());
	options->set_text("options");
	options->set_position({0.0f, -250.0f});
	options->callback = [this] { push_view(std::make_unique<Prefs::View>(&get_app(), get_services())); };

	auto quit = std::make_unique<ui::Button>(get_services());
	quit->set_text("quit");
	quit->set_position({0.0f, -400.0f});
	quit->callback = [this]() { get_app().shutdown(); };

	auto view = std::make_unique<ui::View>(get_services());
	view->push(std::move(m_header));
	view->push(std::move(start));
	view->push(std::move(options));
	view->push(std::move(quit));
	push_view(std::move(view));
}
} // namespace spaced
