#pragma once
#include <bave/graphics/rect.hpp>
#include <bave/graphics/render_device.hpp>
#include <bave/services/service.hpp>

namespace spaced {
struct Layout : bave::IService {
	bave::NotNull<bave::RenderDevice const*> render_device;

	glm::vec2 world_space{};
	bave::Rect<> play_area{};
	bave::Rect<> hud_area{};

	float player_x{-700.0f};
	glm::vec2 player_size{100.0f};

	explicit Layout(bave::NotNull<bave::RenderDevice const*> render_device) : render_device(render_device) {}

	[[nodiscard]] auto project(glm::vec2 fb_point) const -> glm::vec2 { return render_device->project_to(world_space, fb_point); }
};
} // namespace spaced
