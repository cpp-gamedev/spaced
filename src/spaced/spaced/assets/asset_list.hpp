#pragma once
#include <bave/async_exec.hpp>
#include <bave/loader.hpp>
#include <bave/services/resources.hpp>
#include <bave/services/services.hpp>
#include <spaced/assets/asset_manifest.hpp>
#include <set>

namespace bave {
struct Resources;
}

namespace spaced {
class AssetLoader;

class AssetList {
  public:
	explicit AssetList(bave::Loader loader, bave::Services const& services);

	auto add_texture(std::string uri, bool mip_map = false) -> AssetList&;
	auto add_font(std::string uri) -> AssetList&;
	auto add_particle_emitter(std::string uri) -> AssetList&;
	auto add_audio_clip(std::string uri) -> AssetList&;

	void add_manifest(AssetManifest manifest);

	[[nodiscard]] auto build_task_stages() const -> std::vector<bave::AsyncExec::Stage>;

  private:
	struct Tex {
		std::string uri{};
		bool mip_map{};

		// MacOS doesn't provide operator<=> for strings :/
		auto operator==(Tex const& rhs) const -> bool { return uri == rhs.uri; }
		auto operator<(Tex const& rhs) const -> bool { return uri < rhs.uri; }
	};

	auto build_stage_0(AssetLoader& asset_loader) const -> bave::AsyncExec::Stage;
	auto build_stage_1(AssetLoader& asset_loader) const -> bave::AsyncExec::Stage;

	bave::Loader m_loader;
	bave::NotNull<bave::Resources*> m_resources;

	std::set<Tex> m_textures{};
	std::set<std::string> m_fonts{};
	std::set<std::string> m_emitters{};
	std::set<std::string> m_audio_clips{};
};
} // namespace spaced
