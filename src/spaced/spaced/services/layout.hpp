#pragma once
#include <bave/graphics/render_view.hpp>
#include <spaced/services/service.hpp>

namespace spaced {
class ILayout : public IService {
  public:
	[[nodiscard]] virtual auto get_main_view() const -> bave::RenderView const& = 0;
	[[nodiscard]] virtual auto get_framebuffer_size() const -> glm::vec2 = 0;
	[[nodiscard]] virtual auto get_world_space() const -> glm::vec2 = 0;
	[[nodiscard]] virtual auto get_play_area() const -> bave::Rect<> = 0;
	[[nodiscard]] virtual auto get_hud_area() const -> bave::Rect<> = 0;
	[[nodiscard]] virtual auto project_to_world(glm::vec2 fb_point) const -> glm::vec2 = 0;
	[[nodiscard]] virtual auto unproject(glm::vec2 pointer) const -> glm::vec2 = 0;
	[[nodiscard]] virtual auto get_player_x() const -> float = 0;
	[[nodiscard]] virtual auto get_player_size() const -> glm::vec2 = 0;

	virtual void set_framebuffer_size(glm::vec2 size) = 0;
};
} // namespace spaced
