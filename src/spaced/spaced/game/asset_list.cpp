#include <spaced/game/asset_list.hpp>
#include <spaced/game/asset_loader.hpp>
#include <spaced/services/resources.hpp>

namespace spaced {
using bave::Loader;

AssetList::AssetList(Loader loader, Services const& services) : m_loader(std::move(loader)), m_resources(&services.get<Resources>()) {}

auto AssetList::add_texture(std::string uri, bool const mip_map) -> AssetList& {
	m_textures.insert(Tex{.uri = std::move(uri), .mip_map = mip_map});
	return *this;
}

auto AssetList::add_font(std::string uri) -> AssetList& {
	m_fonts.insert(std::move(uri));
	return *this;
}

auto AssetList::add_particle_emitter(std::string uri) -> AssetList& {
	auto const json = m_loader.load_json(uri);
	if (!json) { return *this; }

	// emitters require textures (stage 0) to be loaded, and must be loaded in stage 1
	if (auto const& texture = json["texture"]) { add_texture(texture.as<std::string>()); }
	m_emitters.insert(std::move(uri));
	return *this;
}

auto AssetList::build_task_stages() const -> std::vector<AsyncExec::Stage> {
	auto ret = std::vector<AsyncExec::Stage>{};
	ret.reserve(2);
	auto asset_loader = AssetLoader{m_loader, m_resources};
	ret.push_back(build_stage_0(asset_loader));
	ret.push_back(build_stage_1(asset_loader));
	return ret;
}

auto AssetList::build_stage_0(AssetLoader& asset_loader) const -> AsyncExec::Stage {
	auto ret = AsyncExec::Stage{};
	for (auto const& texture : m_textures) { ret.push_back(asset_loader.make_load_texture(texture.uri, texture.mip_map)); }
	for (auto const& font : m_fonts) { ret.push_back(asset_loader.make_load_font(font)); }
	return ret;
}

auto AssetList::build_stage_1(AssetLoader& asset_loader) const -> AsyncExec::Stage {
	auto ret = AsyncExec::Stage{};
	for (auto const& emitter : m_emitters) { ret.push_back(asset_loader.make_load_particle_emitter(emitter)); }
	return ret;
}
} // namespace spaced
