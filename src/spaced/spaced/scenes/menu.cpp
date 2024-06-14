#include <bave/services/resources.hpp>
#include <bave/services/scene_switcher.hpp>
#include <bave/ui/button.hpp>
#include <bave/ui/dialog.hpp>
#include <bave/ui/text.hpp>
#include <spaced/assets/asset_list.hpp>
#include <spaced/prefs.hpp>
#include <spaced/scenes/game.hpp>
#include <spaced/scenes/menu.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/util.hpp>

namespace spaced {
using bave::App;
using bave::AsyncExec;
using bave::Seconds;
using bave::Services;
using bave::TextHeight;

namespace ui = bave::ui;

MenuScene::MenuScene(App& app, Services const& services) : Scene(app, services, "Home") {}

auto MenuScene::build_load_stages() -> std::vector<AsyncExec::Stage> {
	auto ret = std::vector<AsyncExec::Stage>{};
	auto asset_list = AssetList{make_loader(), get_services()};
	asset_list.add_audio_clip("music/menu.mp3");
	ret.push_back(asset_list.build_load_stage());
	return ret;
}

void MenuScene::on_loaded() {
	switch_track("music/menu.mp3");
	create_ui();
}

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
