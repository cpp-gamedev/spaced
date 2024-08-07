#include <fmt/format.h>
#include <bave/services/resources.hpp>
#include <bave/services/styles.hpp>
#include <bave/ui/button.hpp>
#include <spaced/game/hud.hpp>
#include <spaced/services/game_signals.hpp>
#include <spaced/services/layout.hpp>

namespace spaced {
using bave::Display;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Styles;
using bave::Text;
using bave::TextHeight;
using bave::Texture;

namespace ui = bave::ui;

Hud::Hud(Services const& services) : m_layout(&services.get<Layout>()), m_styles(&services.get<Styles>()) {
	create_background();
	create_score(services);
	create_lives(services);
	create_weapon(services);

	block_input_events = false;
	render_view = services.get<Display>().world.render_view;

	m_on_weapon_changed = services.get<GameSignals>().weapon_changed.connect([this](Weapon const& weapon) { set_weapon(weapon.get_icon()); });
}

void Hud::set_lives(int lives) {
	lives = std::clamp(lives, 0, 99); // TODO: max_lives
	m_lives_count->text.set_string(fmt::format("x{}", lives));
}

void Hud::set_score(std::int64_t const score) { m_score->text.set_string(fmt::format("{}", score)); }

void Hud::set_hi_score(std::int64_t const score) { m_hi_score->text.set_string(fmt::format("HI {}", score)); }

void Hud::set_weapon(std::shared_ptr<Texture const> texture) {
	if (texture) { m_weapon_icon->sprite.set_size(texture->get_size()); }
	m_weapon_icon->sprite.set_texture(std::move(texture));
}

auto Hud::make_text(Services const& services) const -> std::unique_ptr<ui::TextWrap> {
	auto const& rgbas = m_styles->rgbas;
	auto text = std::make_unique<ui::TextWrap>(services);
	text->text.set_height(TextHeight{60});
	text->text.transform.position = m_layout->hud_area.centre();
	text->text.tint = rgbas["grey"];
	return text;
}

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
	auto text = make_text(services);
	m_score = text.get();
	text->text.set_string("9999999999");
	m_text_bounds_size = text->text.get_bounds().size();
	text->text.transform.position.y -= 0.5f * m_text_bounds_size.y;
	set_score(0);
	push(std::move(text));

	text = make_text(services);
	m_hi_score = text.get();
	text->text.transform.position.x = m_layout->hud_area.rb.x - 50.0f;
	text->text.transform.position.y -= 0.5f * m_text_bounds_size.y;
	text->text.set_align(Text::Align::eRight);
	set_hi_score(0);
	push(std::move(text));
}

void Hud::create_lives(Services const& services) {
	auto sprite = std::make_unique<ui::SpriteWrap>();
	m_lives_icon = sprite.get();
	m_lives_icon->sprite.set_size(glm::vec2{20.0f});
	auto const& resources = services.get<Resources>();
	if (auto const texture = resources.get<Texture>("assets/images/icon_player.png")) {
		m_lives_icon->sprite.set_texture(texture);
		m_lives_icon->sprite.set_size(texture->get_size());
	}
	auto position = m_layout->hud_area.centre();
	position.x = m_layout->hud_area.lt.x + 100.0f;
	m_lives_icon->set_position(position);
	push(std::move(sprite));

	auto text = make_text(services);
	text->text.transform.position.y -= 0.5f * m_text_bounds_size.y;
	text->text.transform.position.x = m_lives_icon->get_position().x + m_lives_icon->get_size().x;
	text->text.set_align(Text::Align::eLeft);
	text->text.set_string("0");
	m_lives_count = text.get();
	push(std::move(text));
}

void Hud::create_weapon(Services const& /*services*/) {
	auto sprite = std::make_unique<ui::SpriteWrap>();
	m_weapon_icon = sprite.get();
	sprite->sprite.set_size(glm::vec2{50.0f});
	auto position = m_lives_icon->get_position();
	position.y -= 5.0f;
	position.x = m_lives_count->get_position().x + 200.0f;
	sprite->set_position(position);
	push(std::move(sprite));
}
} // namespace spaced
