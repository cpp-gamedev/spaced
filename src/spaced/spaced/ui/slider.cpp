#include <spaced/services/styles.hpp>
#include <spaced/ui/slider.hpp>
#include <algorithm>

namespace spaced::ui {
using bave::Action;
using bave::MouseButton;
using bave::PointerId;
using bave::PointerMove;
using bave::PointerTap;
using bave::Rect;
using bave::Shader;

Slider::Slider(Services const& services) : m_styles(&services.get<Styles>()), m_progress_bar(services) {
	set_style(m_styles->sliders["default"]);
	set_value(0.0f);
}

auto Slider::get_size() const -> glm::vec2 {
	auto ret = m_progress_bar.get_size();
	ret.y = m_knob.get_shape().diameter;
	return ret;
}

void Slider::set_position(glm::vec2 const position) {
	m_progress_bar.set_position(position);
	set_value(m_value);
}

void Slider::set_value(float const value) {
	update_value(std::clamp(value, 0.0f, 1.0f));
	auto const origin = m_progress_bar.get_position();
	auto const size = m_progress_bar.get_size();
	auto const left_x = origin.x - 0.5f * size.x;
	auto const delta = m_value * size.x;

	m_knob.transform.position = glm::vec2{left_x + delta, origin.y};
	m_progress_bar.set_progress(m_value);

	if (on_change) { on_change(m_value); }
}

void Slider::set_style(Style style) {
	m_style = std::move(style);
	m_knob.tint = m_style.knob_tint;
	auto shape = m_knob.get_shape();
	shape.diameter = m_style.knob_diameter;
	m_knob.set_shape(shape);
	m_progress_bar.set_style(m_styles->progress_bars[m_style.progress_bar]);
}

void Slider::draw(Shader& shader) const {
	m_progress_bar.draw(shader);
	m_knob.draw(shader);
}

void Slider::on_tap(PointerTap const& pointer_tap) {
	if (pointer_tap.pointer.id != PointerId::ePrimary || pointer_tap.button != MouseButton::eLeft) { return; }
	switch (pointer_tap.action) {
	case Action::ePress: m_pointer_down = hit_test(pointer_tap.pointer.position); break;
	case Action::eRelease: m_pointer_down = false; break;
	default: break;
	}

	if (m_pointer_down) { track(pointer_tap.pointer.position); }
}

void Slider::on_move(PointerMove const& pointer_move) {
	if (pointer_move.pointer.id != PointerId::ePrimary) { return; }
	if (!m_pointer_down) { return; }
	track(pointer_move.pointer.position);
}

auto Slider::hit_test(glm::vec2 const position) const -> bool {
	auto const bar_rect = Rect<>::from_size(m_progress_bar.get_size(), m_progress_bar.get_position());
	if (bar_rect.contains(position)) { return true; }
	if (m_knob.get_bounds().contains(position)) { return true; }
	return false;
}

void Slider::track(glm::vec2 const pointer) {
	auto const pointer_x = pointer.x;
	auto const origin_x = m_progress_bar.get_position().x;
	auto const size_x = m_progress_bar.get_size().x;
	auto const left_x = origin_x - 0.5f * size_x;
	auto const right_x = origin_x + 0.5f * size_x;
	auto const knob_x = std::clamp(pointer_x, left_x, right_x);

	m_knob.transform.position.x = knob_x;
	update_value((knob_x - left_x) / size_x);
	m_progress_bar.set_progress(m_value);
}

void Slider::update_value(float const value) {
	if (std::abs(m_value - value) < 0.001f) { return; }
	m_value = value;
	if (on_change) { on_change(m_value); }
}
} // namespace spaced::ui
