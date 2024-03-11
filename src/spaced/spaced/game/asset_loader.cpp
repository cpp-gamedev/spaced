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

auto AssetLoader::make_load_font(std::string uri, bool reload) -> LoadTask {
	auto const load = [](Loader const& loader, std::string_view const uri) { return loader.load_font(uri); };
	return make_load_task(std::move(uri), reload, load);
}

auto AssetLoader::make_load_texture(std::string uri, bool mip_map, bool reload) -> LoadTask {
	auto const load = [mip_map](Loader const& loader, std::string_view const uri) { return loader.load_texture(uri, mip_map); };
	return make_load_task(std::move(uri), reload, load);
}

auto AssetLoader::make_load_texture_atlas(std::string uri, bool mip_map, bool reload) -> LoadTask {
	auto const load = [mip_map](Loader const& loader, std::string_view const uri) { return loader.load_texture_atlas(uri, mip_map); };
	return make_load_task(std::move(uri), reload, load);
}

template <typename FuncT>
auto AssetLoader::make_load_task(std::string uri, bool reload, FuncT load) const -> LoadTask {
	return [impl = m_impl, uri = std::move(uri), reload, load] {
		auto lock = std::unique_lock{impl->mutex};
		if (!reload && impl->resources->contains(uri)) { return; }

		lock.unlock();
		auto asset = load(impl->loader, uri);
		if (!asset) { return; }

		lock.lock();
		impl->resources->add(uri, std::move(asset));
	};
}
} // namespace spaced
