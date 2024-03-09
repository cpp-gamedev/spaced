#pragma once
#include <bave/core/ptr.hpp>
#include <bave/loader.hpp>
#include <spaced/services/resources.hpp>
#include <functional>
#include <memory>

namespace spaced {
class AssetLoader {
  public:
	using LoadTask = std::function<void()>;

	explicit AssetLoader(bave::Loader loader, bave::NotNull<Resources*> resources);

	[[nodiscard]] auto make_load_texture(std::string uri, bool mip_map = false, bool reload = false) -> LoadTask;

  private:
	template <typename MapT, typename FuncT>
	auto make_load_task(MapT& out_map, std::string uri, bool reload, FuncT load) const -> LoadTask;

	struct Impl;
	std::shared_ptr<Impl> m_impl{};
};
} // namespace spaced
