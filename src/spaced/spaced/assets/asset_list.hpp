#pragma once
#include <bave/async_exec.hpp>
#include <bave/loader.hpp>
#include <bave/services/resources.hpp>
#include <bave/services/services.hpp>
#include <spaced/assets/asset_manifest.hpp>
#include <unordered_map>
#include <unordered_set>

namespace bave {
struct Resources;
}

namespace spaced {
class AssetLoader;

class AssetList {
  public:
	explicit AssetList(bave::Loader loader, bave::Services const& services);

	auto add_texture(std::string uri, bool mip_map = false) -> AssetList&;
	auto add_nine_slice(std::string uri) -> AssetList&;
	auto add_atlas(std::string uri, bool mip_map = false) -> AssetList&;
	auto add_font(std::string uri) -> AssetList&;
	auto add_audio_clip(std::string uri) -> AssetList&;
	auto add_anim_timeline(std::string uri) -> AssetList&;
	auto add_particle_emitter(std::string uri) -> AssetList&;

	void add_manifest(AssetManifest manifest);

	[[nodiscard]] auto build_load_stage() const -> bave::AsyncExec::Stage;

  private:
	bave::Loader m_loader;
	bave::NotNull<bave::Resources*> m_resources;

	std::unordered_map<std::string, bool> m_textures{};
	std::unordered_set<std::string> m_nine_slices{};
	std::unordered_map<std::string, bool> m_atlases{};
	std::unordered_set<std::string> m_fonts{};
	std::unordered_set<std::string> m_audio_clips{};
	std::unordered_set<std::string> m_anim_timelines{};
	std::unordered_set<std::string> m_emitters{};
};
} // namespace spaced
