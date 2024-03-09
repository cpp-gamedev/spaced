#pragma once
#include <bave/font/font.hpp>
#include <bave/graphics/texture.hpp>
#include <spaced/resource_map.hpp>
#include <spaced/services/service.hpp>
#include <memory>

namespace spaced {
struct Resources : ResourceMap, IService {
	std::shared_ptr<bave::Font> main_font{};
	std::shared_ptr<bave::Texture> spinner{};
};
} // namespace spaced
