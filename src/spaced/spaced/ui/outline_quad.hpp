#pragma once
#include <bave/graphics/shape.hpp>
#include <spaced/ui/element.hpp>

namespace spaced::ui {
class OutlineQuad : public IElement {
  public:
	OutlineQuad() { set_size(bave::Quad::size_v); }

	void draw(bave::Shader& shader) const final;

	[[nodiscard]] auto get_size() const -> glm::vec2 final { return m_quad.get_shape().size; }
	void set_size(glm::vec2 size);

	[[nodiscard]] auto get_position() const -> glm::vec2 final { return m_quad.transform.position; }
	void set_position(glm::vec2 position) final;

	void set_scale(glm::vec2 scale);

	void set_corner_ratio(float ratio);
	void set_corner_resolution(int resolution);
	void set_outline_width(float width);
	void set_outline_tint(bave::Rgba tint);
	void set_tint(bave::Rgba tint);

	[[nodiscard]] auto get_background() const -> bave::RoundedQuadShape const& { return m_quad; }
	[[nodiscard]] auto get_outline() const -> bave::RoundedQuadShape const& { return m_outline; }

  private:
	template <typename F>
	void set_quad_and_outline(F func);

	bave::RoundedQuadShape m_quad{};
	bave::RoundedQuadShape m_outline{};
	float m_outline_width{10.0f};
	float m_corner_ratio{0.0f};
};
} // namespace spaced::ui
