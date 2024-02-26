#include <spaced/ui/outline_quad.hpp>

namespace spaced::ui {
using bave::Rgba;
using bave::RoundedQuad;
using bave::Shader;

void OutlineQuad::draw(Shader& shader) const {
	m_outline.draw(shader);
	m_quad.draw(shader);
}

void OutlineQuad::set_size(glm::vec2 const size) {
	set_quad_and_outline([size, this](RoundedQuad& quad, RoundedQuad& outline) {
		quad.size = size;
		quad.corner_radius = m_corner_ratio * std::min(quad.size.x, quad.size.y);
		outline.size = size + m_outline_width;
		outline.corner_radius = m_corner_ratio * std::min(outline.size.x, outline.size.y);
	});
}

void OutlineQuad::set_position(glm::vec2 const position) { m_quad.transform.position = m_outline.transform.position = position; }

void OutlineQuad::set_scale(glm::vec2 const scale) { m_quad.transform.scale = m_outline.transform.scale = scale; }

void OutlineQuad::set_corner_ratio(float const ratio) {
	m_corner_ratio = ratio;
	set_size(m_quad.get_shape().size);
}

void OutlineQuad::set_corner_resolution(int const resolution) {
	set_quad_and_outline([resolution](RoundedQuad& quad, RoundedQuad& outline) { quad.corner_resolution = outline.corner_resolution = resolution; });
}

void OutlineQuad::set_outline_width(float const width) {
	m_outline_width = width;
	set_size(m_quad.get_shape().size);
}

void OutlineQuad::set_outline_tint(Rgba const tint) { m_outline.tint = tint; }

void OutlineQuad::set_tint(Rgba const tint) { m_quad.tint = tint; }

template <typename F>
void OutlineQuad::set_quad_and_outline(F func) {
	auto quad_shape = m_quad.get_shape();
	auto outline_shape = m_outline.get_shape();
	func(quad_shape, outline_shape);
	m_quad.set_shape(quad_shape);
	m_outline.set_shape(outline_shape);
}
} // namespace spaced::ui
