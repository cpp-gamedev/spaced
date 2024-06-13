#include <bave/services/resources.hpp>
#include <bave/services/scene_switcher.hpp>
#include <spaced/assets/asset_list.hpp>
#include <spaced/scenes/game.hpp>
#include <spaced/scenes/load_assets.hpp>
#include <spaced/scenes/menu.hpp>
#include <spaced/util.hpp>

namespace spaced {
using bave::App;
using bave::AsyncExec;
using bave::Loader;
using bave::Resources;
using bave::Services;

namespace {
auto make_load_stages(Loader loader, Services const& services) -> std::vector<AsyncExec::Stage> {
	auto asset_list = AssetList{std::move(loader), services};
	asset_list.add_manifest(GameScene::get_manifest());
	auto ret = asset_list.build_task_stages();
	auto& stage = ret.emplace_back();
	auto& resources = services.get<Resources>();
	stage.push_back(util::create_font_atlas_task(resources.main_font, MenuScene::get_text_heights()));
	return ret;
}
} // namespace

LoadAssets::LoadAssets(App& app, Services const& services) : Scene(app, services, "LoadAssets") {}

auto LoadAssets::build_load_stages() -> std::vector<AsyncExec::Stage> { return make_load_stages(make_loader(), get_services()); }

void LoadAssets::on_loaded() { get_switcher().switch_to<MenuScene>(); }
} // namespace spaced
