#pragma once
#include <bave/graphics/rect.hpp>
#include <spaced/services/service.hpp>

namespace spaced {
struct Layout : IService {
	glm::vec2 world_space{};
	bave::Rect<> play_area{};
	bave::Rect<> hud_area{};

	float player_x{-700.0f};
	glm::vec2 player_size{100.0f};
};
} // namespace spaced
