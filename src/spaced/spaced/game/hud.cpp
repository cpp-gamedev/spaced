#include <fmt/format.h>
#include <bave/services/styles.hpp>
#include <bave/ui/button.hpp>
#include <spaced/game/hud.hpp>
#include <spaced/services/layout.hpp>

namespace spaced {
using bave::IDisplay;
using bave::Seconds;
using bave::Services;
using bave::Styles;
using bave::TextHeight;

namespace ui = bave::ui;

Hud::Hud(Services const& services)
	: ui::View(services), m_display(&services.get<IDisplay>()), m_layout(&services.get<Layout>()), m_styles(&services.get<Styles>()) {
	create_background();
	create_score(services);

	block_input_events = false;
	render_view = m_display->get_world_view();
}

void Hud::set_score(std::int64_t const score) { m_score->text.set_string(fmt::format("{}", score)); }

void Hud::set_hi_score(std::int64_t const score) { m_hi_score->text.set_string(fmt::format("HI {}", score)); }

void Hud::create_background() {
	auto background = std::make_unique<ui::OutlineQuad>();
	m_background = background.get();
	background->set_outline_width(0.0f);
	background->set_size(m_layout->hud_area.size());
	background->set_position(m_layout->hud_area.centre());
	background->set_tint(m_styles->rgbas["milk"]);
	push(std::move(background));
}

void Hud::create_score(Services const& services) {
	auto const& rgbas = m_styles->rgbas;

	auto make_text = [&] {
		auto text = std::make_unique<ui::Text>(services);
		text->text.set_height(TextHeight{60});
		text->text.transform.position = m_layout->hud_area.centre();
		text->text.tint = rgbas["grey"];
		return text;
	};

	auto text = make_text();
	m_score = text.get();
	text->text.set_string("9999999999");
	m_text_bounds_size = text->text.get_bounds().size();
	text->text.transform.position.y -= 0.5f * m_text_bounds_size.y;
	set_score(0);

	push(std::move(text));

	text = make_text();
	m_hi_score = text.get();
	text->text.transform.position.x = m_layout->hud_area.rb.x - 50.0f;
	text->text.transform.position.y -= 0.5f * m_text_bounds_size.y;
	text->text.set_align(bave::Text::Align::eLeft);
	set_hi_score(0);

	push(std::move(text));
}
} // namespace spaced
