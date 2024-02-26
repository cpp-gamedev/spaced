#include <spaced/services/styles.hpp>
#include <spaced/ui/progress_bar.hpp>

namespace spaced::ui {
using bave::Shader;

ProgressBar::ProgressBar(Services const& services) : m_style(services.get<Styles>().get_progress_bar("default")) {}

void ProgressBar::set_progress(float const progress) {
	m_progress = progress;
	auto const width = progress * size.x;

	m_background.set_size(size);
	m_background.set_corner_ratio(0.0f);

	auto quad = bave::RoundedQuad{};
	quad.size = size;
	quad.corner_radius = 0.0f;
	quad.size.x = width;
	m_fill.set_shape(quad);

	m_background.set_tint(m_style.background);
	m_background.set_outline_tint(m_style.outline);
	m_fill.tint = m_style.fill;

	m_fill.tint.channels.w = 0xff;

	m_background.set_position(position);
	m_fill.transform.position = position;
	m_fill.transform.position.x += 0.5f * (-size.x + width);
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
