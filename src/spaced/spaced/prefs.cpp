#include <bave/persistor.hpp>
#include <bave/services/audio.hpp>
#include <bave/services/styles.hpp>
#include <bave/ui/button.hpp>
#include <bave/ui/slider.hpp>
#include <bave/ui/text.hpp>
#include <djson/json.hpp>
#include <spaced/prefs.hpp>

namespace spaced {
using bave::App;
using bave::IAudio;
using bave::NotNull;
using bave::Persistor;
using bave::Services;
using bave::Styles;

namespace ui = bave::ui;

namespace {
constexpr std::string_view uri_v{"userdata/prefs.json"};
} // namespace

auto Prefs::load(App const& app) -> Prefs {
	auto ret = Prefs{};
	auto const persistor = Persistor{app};
	if (persistor.exists(uri_v)) {
		auto const json = persistor.read_json(uri_v);
		if (auto const& music_gain = json["music_gain"]) { from_json(music_gain, ret.music_gain); }
		if (auto const& sfx_gain = json["sfx_gain"]) { from_json(sfx_gain, ret.sfx_gain); }
		if (auto const& starfield_density = json["starfield_density"]) { from_json(starfield_density, ret.starfield_density); }
	}
	return ret;
}

void Prefs::save(App const& app) const {
	auto json = dj::Json{};
	to_json(json["music_gain"], music_gain);
	to_json(json["sfx_gain"], sfx_gain);
	to_json(json["starfield_density"], starfield_density);
	auto const persistor = Persistor{app};
	persistor.write_json(uri_v, json);
}

Prefs::View::View(NotNull<App const*> app, Services const& services) : m_app(app), m_audio(&services.get<IAudio>()), m_prefs(Prefs::load(*app)) {
	auto const& styles = services.get<Styles>();

	auto bg = std::make_unique<ui::OutlineQuad>();
	bg->set_size({600.0f, 600.0f});
	bg->set_outline_width(5.0f);
	bg->set_corner_ratio(0.1f);
	bg->set_tint(styles.rgbas["milk"]);
	push(std::move(bg));

	auto text = std::make_unique<ui::Text>(services);
	text->text.set_string("music");
	text->set_position({0.0f, 230.0f});
	push(std::move(text));

	auto slider = std::make_unique<ui::Slider>(services);
	slider->set_value(m_prefs.music_gain);
	slider->on_change = [this](float const val) {
		m_prefs.music_gain = val;
		m_audio->set_music_gain(val);
	};
	slider->set_position({0.0f, 200.0f});
	push(std::move(slider));

	text = std::make_unique<ui::Text>(services);
	text->text.set_string("sfx");
	text->set_position({0.0f, 130.0f});
	push(std::move(text));

	slider = std::make_unique<ui::Slider>(services);
	slider->set_value(m_prefs.sfx_gain);
	slider->on_change = [this](float const val) {
		m_prefs.sfx_gain = val;
		m_audio->set_sfx_gain(val);
	};
	slider->set_position({0.0f, 100.0f});
	push(std::move(slider));

	text = std::make_unique<ui::Text>(services);
	text->text.set_string("starfield density");
	text->set_position({0.0f, 30.0f});
	push(std::move(text));

	slider = std::make_unique<ui::Slider>(services);
	slider->set_value(m_prefs.starfield_density);
	slider->on_change = [this](float const val) { m_prefs.starfield_density = val; };
	slider->set_position({0.0f, 0.0f});
	push(std::move(slider));

	auto button = std::make_unique<ui::Button>(services);
	button->set_text("close");
	button->callback = [this] { set_destroyed(); };
	button->set_position({0.0f, -200.0f});
	push(std::move(button));
}

Prefs::View::~View() { m_prefs.save(*m_app); }
} // namespace spaced
