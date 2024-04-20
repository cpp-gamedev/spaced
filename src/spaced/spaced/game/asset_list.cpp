#include <spaced/game/asset_list.hpp>
#include <spaced/game/asset_loader.hpp>
#include <spaced/services/resources.hpp>

namespace spaced {
using bave::Loader;

AssetList::AssetList(Loader loader, Services const& services) : m_loader(std::move(loader)), m_resources(&services.get<Resources>()) {}

auto AssetList::add_texture(std::string uri, bool const mip_map) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_textures.insert(Tex{.uri = std::move(uri), .mip_map = mip_map});
	return *this;
}

auto AssetList::add_font(std::string uri) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_fonts.insert(std::move(uri));
	return *this;
}

auto AssetList::add_particle_emitter(std::string uri) -> AssetList& {
	if (uri.empty()) { return *this; }

	auto const json = m_loader.load_json(uri);
	if (!json) { return *this; }

	// emitters require textures (stage 0) to be loaded, and must be loaded in stage 1
	if (auto const& texture = json["texture"]) { add_texture(texture.as<std::string>()); }
	m_emitters.insert(std::move(uri));
	return *this;
}

auto AssetList::add_audio_clip(std::string uri) -> AssetList& {
	if (uri.empty()) { return *this; }
	m_audio_clips.insert(std::move(uri));
	return *this;
}

auto AssetList::read_world_spec(std::string_view const uri) -> WorldSpec {
	if (uri.empty()) { return {}; }

	auto const json = m_loader.load_json(uri);
	if (!json) { return {}; }

	auto ret = WorldSpec{};
	ret.name = json["name"].as_string();
	ret.background_tint = json["background_tint"].as_string();

	if (auto const& player = json["player"]) {
		ret.player.tint = player["tint"].as_string();
		ret.player.exhaust_emitter = player["exhaust_emitter"].as_string();
		ret.player.death_emitter = player["death_emitter"].as_string();
		add_particle_emitter(ret.player.exhaust_emitter);
		add_particle_emitter(ret.player.death_emitter);
	}

	for (auto const& enemy_factory : json["enemy_factories"].array_view()) {
		add_particle_emitter(enemy_factory["death_emitter"].as<std::string>());
		for (auto const& death_sfx : enemy_factory["death_sfx"].array_view()) { add_audio_clip(death_sfx.as<std::string>()); }
		ret.enemy_factories.push_back(enemy_factory);
	}

	return ret;
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
	for (auto const& audio_clip : m_audio_clips) { ret.push_back(asset_loader.make_load_audio_clip(audio_clip)); }
	return ret;
}

auto AssetList::build_stage_1(AssetLoader& asset_loader) const -> AsyncExec::Stage {
	auto ret = AsyncExec::Stage{};
	for (auto const& emitter : m_emitters) { ret.push_back(asset_loader.make_load_particle_emitter(emitter)); }
	return ret;
}
} // namespace spaced
