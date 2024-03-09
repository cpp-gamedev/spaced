#pragma once
#include <bave/font/font.hpp>
#include <bave/graphics/texture_atlas.hpp>
#include <spaced/services/service.hpp>
#include <spaced/string_map.hpp>
#include <memory>

namespace spaced {
struct Resources : IService {
	std::shared_ptr<bave::Font> main_font{};
	std::shared_ptr<bave::Texture> spinner{};

	StringMap<std::shared_ptr<bave::Texture>> textures{};
	StringMap<std::shared_ptr<bave::TextureAtlas>> atlases{};
};
} // namespace spaced
