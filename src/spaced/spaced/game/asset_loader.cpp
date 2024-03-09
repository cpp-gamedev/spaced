#include <bave/logger.hpp>
#include <spaced/game/asset_loader.hpp>
#include <mutex>

namespace spaced {
using bave::Loader;
using bave::Logger;
using bave::NotNull;

struct AssetLoader::Impl {
	Logger log{"AssetLoader"};
	Loader loader;
	NotNull<Resources*> resources;
	std::mutex mutex{};
};

AssetLoader::AssetLoader(Loader loader, NotNull<Resources*> resources) : m_impl(new Impl{.loader = std::move(loader), .resources = resources}) {}

auto AssetLoader::make_load_texture(std::string uri, bool mip_map, bool reload) -> LoadTask {
	auto const load = [mip_map](Loader const& loader, std::string_view const uri) { return loader.load_texture(uri, mip_map); };
	return make_load_task(m_impl->resources->textures, std::move(uri), reload, load);
}

template <typename MapT, typename FuncT>
auto AssetLoader::make_load_task(MapT& out_map, std::string uri, bool reload, FuncT load) const -> LoadTask {
	return [impl = m_impl, &out_map, uri = std::move(uri), reload, load] {
		auto lock = std::unique_lock{impl->mutex};

		if (!reload && out_map.contains(uri)) {
			lock.unlock();
			return;
		}

		lock.unlock();
		auto asset = load(impl->loader, uri);
		if (!asset) { return; }

		lock.lock();
		out_map.insert_or_assign(uri, std::move(asset));
	};
}
} // namespace spaced
