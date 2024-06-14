#include <djson/json.hpp>
#include <spaced/assets/asset_manifest.hpp>

namespace spaced {
auto AssetManifest::load_from(dj::Json const& json) -> AssetManifest {
	auto ret = AssetManifest{};
	for (auto const& texture : json["textures"].array_view()) { ret.textures.push_back(texture.as<std::string>()); }
	for (auto const& texture : json["flat_textures"].array_view()) { ret.flat_textures.push_back(texture.as<std::string>()); }
	for (auto const& atlas : json["atlases"].array_view()) { ret.atlases.push_back(atlas.as<std::string>()); }
	for (auto const& atlas : json["flat_atlases"].array_view()) { ret.flat_atlases.push_back(atlas.as<std::string>()); }
	for (auto const& slice : json["nine_slices"].array_view()) { ret.nine_slices.push_back(slice.as<std::string>()); }
	for (auto const& font : json["fonts"].array_view()) { ret.fonts.push_back(font.as<std::string>()); }
	for (auto const& timeline : json["anim_timelines"].array_view()) { ret.anim_timelines.push_back(timeline.as<std::string>()); }
	for (auto const& clip : json["audio_clips"].array_view()) { ret.audio_clips.push_back(clip.as<std::string>()); }
	for (auto const& emitter : json["particle_emitters"].array_view()) { ret.particle_emitters.push_back(emitter.as<std::string>()); }
	return ret;
}
} // namespace spaced
