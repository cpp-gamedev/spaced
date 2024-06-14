#pragma once
#include <bave/core/ptr.hpp>
#include <bave/loader.hpp>
#include <bave/services/resources.hpp>
#include <functional>
#include <memory>

namespace spaced {
class AssetLoader {
  public:
	using LoadTask = std::function<void()>;

	explicit AssetLoader(bave::Loader loader, bave::NotNull<bave::Resources*> resources);

	[[nodiscard]] auto make_load_texture(std::string uri, bool mip_map = false, bool reload = false) -> LoadTask;
	[[nodiscard]] auto make_load_nine_slice(std::string uri, bool reload = false) -> LoadTask;
	[[nodiscard]] auto make_load_atlas(std::string uri, bool mip_map = false, bool reload = false) -> LoadTask;
	[[nodiscard]] auto make_load_font(std::string uri, bool reload = false) -> LoadTask;
	[[nodiscard]] auto make_load_audio_clip(std::string uri, bool reload = false) -> LoadTask;
	[[nodiscard]] auto make_load_anim_timeline(std::string uri, bool reload = false) -> LoadTask;
	[[nodiscard]] auto make_load_particle_emitter(std::string uri, bool reload = false) -> LoadTask;

  private:
	template <typename FuncT>
	auto make_load_task(std::string uri, bool reload, FuncT load) const -> LoadTask;

	struct Impl;
	std::shared_ptr<Impl> m_impl{};
};
} // namespace spaced
