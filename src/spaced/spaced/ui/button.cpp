#include <spaced/services/layout.hpp>
#include <spaced/services/resources.hpp>
#include <spaced/services/styles.hpp>
#include <spaced/ui/button.hpp>
#include <algorithm>

namespace spaced::ui {
using bave::Action;
using bave::MouseButton;
using bave::PointerId;
using bave::PointerMove;
using bave::PointerTap;
using bave::RoundedQuad;
using bave::Seconds;
using bave::Shader;
using bave::TextHeight;

Button::Button(Services const& services) : m_display(&services.get<IDisplay>()), m_styles(&services.get<Styles>()) {
	m_text.set_font(services.get<Resources>().main_font);
	set_text_height(TextHeight::eDefault);
	set_style(m_styles->buttons["default"]);
}

void Button::set_position(glm::vec2 const position) {
	m_background.set_position(position);
	m_text.transform.position = position;
	reposition_text();
}

void Button::set_scale(glm::vec2 const scale) {
	m_background.set_scale(scale);
	m_text.transform.scale = scale;
}

void Button::set_font(std::shared_ptr<bave::Font> font) {
	m_text.set_font(std::move(font));
	resize_background();
}

void Button::set_text(std::string text, bool const resize) {
	m_text.set_string(std::move(text));
	if (resize) { resize_background(); }
}

void Button::set_text_height(TextHeight const height, bool const resize) {
	m_text.set_height(height);
	if (resize) { resize_background(); }
	reposition_text();
}

void Button::set_size(glm::vec2 const size) {
	auto const ratio = std::clamp(m_style.corner_ratio, 0.0f, 0.5f);
	auto quad = RoundedQuad{};
	quad.size = glm::max(size, min_size_v);
	quad.corner_resolution = 16;
	m_background.set_size(glm::max(size, min_size_v));
	m_background.set_corner_resolution(16);
	m_background.set_corner_ratio(ratio);
	m_background.set_outline_width(m_style.outline_width);
}

void Button::set_style(Style const& style) {
	m_style = style;
	resize_background();
}

void Button::on_move(PointerMove const& pointer_move) {
	if (pointer_move.pointer.id != PointerId::ePrimary) { return; }
	auto const position = m_display->unproject(pointer_move.pointer.position);
	if (m_hitbox.contains(position)) {
		m_state = State::eHover;
	} else {
		m_state = State::eIdle;
	}
}

void Button::on_tap(PointerTap const& pointer_tap) {
	if (pointer_tap.pointer.id != PointerId::ePrimary || pointer_tap.button != MouseButton::eLeft) { return; }

	auto const position = m_display->unproject(pointer_tap.pointer.position);
	if (!m_hitbox.contains(position)) {
		m_state = State::eIdle;
		return;
	}

	if (pointer_tap.action == Action::ePress) {
		m_state = State::ePress;
		return;
	}

	if (pointer_tap.action == Action::eRelease) {
		m_state = State::eHover;
		if (callback) { callback(); }
	}
}

void Button::tick(Seconds const /*dt*/) {
	m_hitbox = m_background.get_background().get_bounds();

	m_background.set_outline_tint(m_style.outline[m_state]);
	m_background.set_tint(m_style.background[m_state]);
	m_text.tint = m_style.text[m_state];
}

void Button::draw(Shader& shader) const {
	m_background.draw(shader);
	m_text.draw(shader);
}

void Button::resize_background() { set_size(glm::vec2{1.2f, 2.5f} * m_text.get_bounds().size()); }

void Button::reposition_text() { m_text.transform.position.y -= 0.33f * static_cast<float>(m_text.get_height()); }
} // namespace spaced::ui
