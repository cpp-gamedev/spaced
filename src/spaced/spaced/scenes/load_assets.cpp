#include <spaced/assets/asset_list.hpp>
#include <spaced/scenes/game.hpp>
#include <spaced/scenes/load_assets.hpp>
#include <spaced/scenes/menu.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/scene_switcher.hpp>
#include <spaced/util.hpp>

namespace spaced {
using bave::App;
using bave::Loader;
using bave::Seconds;
using bave::Shader;

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

LoadAssets::LoadAssets(App& app, Services const& services)
	: Scene(app, services, "LoadAssets"), m_loading_screen(services), m_load(make_load_stages(make_loader(), services)) {}

void LoadAssets::on_loaded() { get_services().get<ISceneSwitcher>().switch_to<MenuScene>(); }

void LoadAssets::tick(Seconds const dt) {
	auto const load_status = m_load.update();
	auto const progress = load_status.progress();
	m_loading_screen.update(dt, progress);

	if (load_status.is_complete()) { on_loaded(); }
}

void LoadAssets::render(Shader& shader) const { m_loading_screen.draw(shader); }
} // namespace spaced
