#include <spaced/services/resources.hpp>
#include <spaced/services/styles.hpp>
#include <spaced/ui/loading_screen.hpp>

namespace spaced::ui {
using bave::Seconds;
using bave::Shader;

LoadingScreen::LoadingScreen(Services const& services)
	: m_layout(&services.get<ILayout>()), m_style(services.get<Styles>().loading_screen), m_progress_bar(services) {
	auto const world_space = m_layout->get_world_space();

	m_background.set_size(1.1f * world_space);
	m_background.tint = m_style.background_tint;

	m_spinner.set_size(m_style.spinner.size);
	m_spinner.set_texture(services.get<Resources>().spinner);

	m_progress_bar.size = {m_style.progress_bar.n_width * world_space.x, m_style.progress_bar.height};
	m_progress_bar.position.y = -0.5f * world_space.y + m_style.progress_bar.bottom_offset;
	auto style = m_progress_bar.get_style();
	style.padding = m_style.progress_bar.padding;
	m_progress_bar.set_style(style);
}

void LoadingScreen::update(Seconds const dt, float const progress) {
	m_progress_bar.set_progress(progress);

	bave::Degrees rotation = m_spinner.transform.rotation;
	rotation.value -= m_style.spinner.spin_rate.value * dt.count();
	m_spinner.transform.rotation = rotation;
}

void LoadingScreen::draw(Shader& shader) const {
	shader.set_render_view(m_layout->get_main_view());
	m_background.draw(shader);
	m_spinner.draw(shader);
	m_progress_bar.draw(shader);
}
} // namespace spaced::ui
