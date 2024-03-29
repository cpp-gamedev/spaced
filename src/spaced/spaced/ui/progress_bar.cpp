#include <spaced/services/styles.hpp>
#include <spaced/ui/progress_bar.hpp>

namespace spaced::ui {
using bave::Shader;

ProgressBar::ProgressBar(Services const& services) : m_style(services.get<Styles>().progress_bars["default"]) {}

void ProgressBar::set_progress(float const progress) {
	m_progress = progress;
	auto const width = (1.0f - progress) * size.x;

	auto bg_size = size;
	bg_size.x += m_style.corner_ratio * bg_size.y;
	auto quad = bave::RoundedQuad{};
	quad.size = bg_size;
	quad.corner_radius = m_style.corner_ratio * std::min(bg_size.x, bg_size.y);
	m_background.set_shape(quad);

	quad.size = size - m_style.padding;
	quad.corner_radius = 0.0f;
	quad.size.x = width;
	m_fill.set_shape(quad);

	m_background.tint = m_style.background;
	m_fill.tint = m_style.fill;

	m_fill.tint.channels.w = 0xff;

	m_background.transform.position = m_fill.transform.position = position;
	m_fill.transform.position.x += 0.5f * (size.x - width);
}

void ProgressBar::set_style(Style style) {
	m_style = style;
	set_progress(m_progress);
}

void ProgressBar::draw(Shader& shader) const {
	m_background.draw(shader);
	m_fill.draw(shader);
}
} // namespace spaced::ui
