#pragma once
#include <bave/loader.hpp>
#include <spaced/async_exec.hpp>
#include <spaced/game/world_spec.hpp>
#include <spaced/services/services.hpp>
#include <set>

namespace spaced {
struct Resources;
class AssetLoader;

class AssetList {
  public:
	explicit AssetList(bave::Loader loader, Services const& services);

	auto add_texture(std::string uri, bool mip_map = false) -> AssetList&;
	auto add_font(std::string uri) -> AssetList&;
	auto add_particle_emitter(std::string uri) -> AssetList&;

	auto read_world_spec(std::string_view uri) -> WorldSpec;

	[[nodiscard]] auto build_task_stages() const -> std::vector<AsyncExec::Stage>;

  private:
	struct Tex {
		std::string uri{};
		bool mip_map{};

		// MacOS doesn't provide operator<=> for strings :/
		auto operator==(Tex const& rhs) const -> bool { return uri == rhs.uri; }
		auto operator<(Tex const& rhs) const -> bool { return uri < rhs.uri; }
	};

	auto build_stage_0(AssetLoader& asset_loader) const -> AsyncExec::Stage;
	auto build_stage_1(AssetLoader& asset_loader) const -> AsyncExec::Stage;

	bave::Loader m_loader;
	bave::NotNull<Resources*> m_resources;

	std::set<Tex> m_textures{};
	std::set<std::string> m_fonts{};
	std::set<std::string> m_emitters{};
};
} // namespace spaced
