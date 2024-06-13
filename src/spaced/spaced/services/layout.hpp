#pragma once
#include <bave/graphics/render_view.hpp>
#include <spaced/services/service.hpp>

namespace spaced {
class ILayout : public IService {
  public:
	[[nodiscard]] virtual auto get_main_view() const -> bave::RenderView const& = 0;
	[[nodiscard]] virtual auto get_framebuffer_size() const -> glm::vec2 = 0;
	[[nodiscard]] virtual auto get_world_space() const -> glm::vec2 = 0;
	[[nodiscard]] virtual auto project_to_world(glm::vec2 fb_point) const -> glm::vec2 = 0;
	[[nodiscard]] virtual auto unproject(glm::vec2 pointer) const -> glm::vec2 = 0;
};

struct GameLayout : IService {
	glm::vec2 world_space{};
	bave::Rect<> play_area{};
	bave::Rect<> hud_area{};

	float player_x{-700.0f};
	glm::vec2 player_size{100.0f};
};
} // namespace spaced
