#include <fmt/format.h>
#include <spaced/game/hud.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::TextHeight;

Hud::Hud(Services const& services) : ui::View(services), m_styles(&services.get<Styles>()), m_area(m_layout->get_hud_area()) {
	create_background();
	create_score(services);

	block_input_events = false;
}

void Hud::set_score(std::int64_t const score) { m_score->text.set_string(fmt::format("{}", score)); }

void Hud::create_background() {
	auto background = std::make_unique<ui::OutlineQuad>();
	m_background = background.get();
	background->set_outline_width(0.0f);
	background->set_size(m_area.size());
	background->set_position(m_area.centre());
	background->set_tint(m_styles->rgbas["milk"]);
	push(std::move(background));
}

void Hud::create_score(Services const& services) {
	auto const& rgbas = m_styles->rgbas;

	auto text = std::make_unique<ui::Text>(services);
	m_score = text.get();
	text->text.set_height(TextHeight{60});
	text->text.set_string("9999999999");
	auto const text_bounds = text->text.get_bounds();
	auto const text_bounds_size = text_bounds.size();
	text->text.transform.position = m_area.centre();
	text->text.transform.position.y -= 0.5f * text_bounds_size.y;
	text->text.set_string("0");
	text->text.tint = rgbas["grey"];

	push(std::move(text));
}
} // namespace spaced
