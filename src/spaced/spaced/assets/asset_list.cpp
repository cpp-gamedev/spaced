#include <bave/services/resources.hpp>
#include <spaced/assets/asset_list.hpp>
#include <spaced/assets/asset_loader.hpp>

namespace spaced {
using bave::AsyncExec;
using bave::Loader;
using bave::Resources;
using bave::Services;

AssetList::AssetList(Loader loader, Services const& services) : m_loader(std::move(loader)), m_resources(&services.get<Resources>()) {}

auto AssetList::add_texture(std::string uri, bool const mip_map) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_textures.insert_or_assign(std::move(uri), mip_map);
	return *this;
}

auto AssetList::add_nine_slice(std::string uri) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_nine_slices.insert(std::move(uri));
	return *this;
}

auto AssetList::add_atlas(std::string uri, bool const mip_map) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_atlases.insert_or_assign(std::move(uri), mip_map);
	return *this;
}

auto AssetList::add_font(std::string uri) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_fonts.insert(std::move(uri));
	return *this;
}

auto AssetList::add_audio_clip(std::string uri) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_audio_clips.insert(std::move(uri));
	return *this;
}

auto AssetList::add_anim_timeline(std::string uri) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_anim_timelines.insert(std::move(uri));
	return *this;
}

auto AssetList::add_particle_emitter(std::string uri) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_emitters.insert(std::move(uri));
	return *this;
}

void AssetList::add_manifest(AssetManifest manifest) {
	for (auto& uri : manifest.flat_textures) { add_texture(std::move(uri), false); }
	for (auto& uri : manifest.textures) { add_texture(std::move(uri), true); }
	for (auto& uri : manifest.nine_slices) { add_nine_slice(std::move(uri)); }
	for (auto& uri : manifest.flat_atlases) { add_atlas(std::move(uri), false); }
	for (auto& uri : manifest.atlases) { add_atlas(std::move(uri), true); }
	for (auto& uri : manifest.fonts) { add_font(std::move(uri)); }
	for (auto& uri : manifest.audio_clips) { add_audio_clip(std::move(uri)); }
	for (auto& uri : manifest.anim_timelines) { add_anim_timeline(std::move(uri)); }
	for (auto& uri : manifest.particle_emitters) { add_particle_emitter(std::move(uri)); }
}

auto AssetList::build_load_stage() const -> AsyncExec::Stage {
	auto asset_loader = AssetLoader{m_loader, m_resources};
	auto ret = AsyncExec::Stage{};
	for (auto const& [uri, mip_map] : m_textures) { ret.push_back(asset_loader.make_load_texture(uri, mip_map)); }
	for (auto const& uri : m_nine_slices) { ret.push_back(asset_loader.make_load_nine_slice(uri)); }
	for (auto const& [uri, mip_map] : m_atlases) { ret.push_back(asset_loader.make_load_atlas(uri, mip_map)); }
	for (auto const& font : m_fonts) { ret.push_back(asset_loader.make_load_font(font)); }
	for (auto const& audio_clip : m_audio_clips) { ret.push_back(asset_loader.make_load_audio_clip(audio_clip)); }
	for (auto const& anim_timeline : m_anim_timelines) { ret.push_back(asset_loader.make_load_anim_timeline(anim_timeline)); }
	for (auto const& emitter : m_emitters) { ret.push_back(asset_loader.make_load_particle_emitter(emitter)); }
	return ret;
}
} // namespace spaced
