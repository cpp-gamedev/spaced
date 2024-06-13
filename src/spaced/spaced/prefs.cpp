#include <bave/persistor.hpp>
#include <djson/json.hpp>
#include <spaced/prefs.hpp>
#include <spaced/services/audio.hpp>
#include <spaced/services/styles.hpp>
#include <spaced/ui/button.hpp>
#include <spaced/ui/slider.hpp>
#include <spaced/ui/text.hpp>

namespace spaced {
using bave::App;
using bave::NotNull;
using bave::Persistor;

namespace {
constexpr std::string_view uri_v{"spaced/prefs.json"};
} // namespace

auto Prefs::load(App const& app) -> Prefs {
	auto ret = Prefs{};
	auto const persistor = Persistor{app};
	if (persistor.exists(uri_v)) {
		auto const json = persistor.read_json(uri_v);
		from_json(json["music_gain"], ret.music_gain);
		from_json(json["sfx_gain"], ret.sfx_gain);
	}
	return ret;
}

void Prefs::save(App const& app) const {
	auto json = dj::Json{};
	to_json(json["music_gain"], music_gain);
	to_json(json["sfx_gain"], sfx_gain);
	auto const persistor = Persistor{app};
	persistor.write_json(uri_v, json);
}

Prefs::View::View(NotNull<App const*> app, Services const& services)
	: ui::View(services), m_app(app), m_audio(&services.get<IAudio>()), m_prefs(Prefs::load(*app)) {
	auto const& styles = services.get<Styles>();

	auto bg = std::make_unique<ui::OutlineQuad>();
	bg->set_size({400.0f, 400.0f});
	bg->set_outline_width(5.0f);
	bg->set_corner_ratio(0.1f);
	bg->set_tint(styles.rgbas["milk"]);
	push(std::move(bg));

	auto text = std::make_unique<ui::Text>(services);
	text->text.set_string("music");
	text->set_position({0.0f, 130.0f});
	push(std::move(text));

	auto slider = std::make_unique<ui::Slider>(services);
	slider->set_value(m_prefs.music_gain);
	slider->on_change = [this](float const val) {
		m_prefs.music_gain = val;
		m_audio->set_music_gain(val);
	};
	slider->set_position({0.0f, 100.0f});
	push(std::move(slider));

	text = std::make_unique<ui::Text>(services);
	text->text.set_string("sfx");
	text->set_position({0.0f, 30.0f});
	push(std::move(text));

	slider = std::make_unique<ui::Slider>(services);
	slider->set_value(m_prefs.sfx_gain);
	slider->on_change = [this](float const val) {
		m_prefs.sfx_gain = val;
		m_audio->set_sfx_gain(val);
	};
	slider->set_position({0.0f, 0.0f});
	push(std::move(slider));

	auto button = std::make_unique<ui::Button>(services);
	button->set_text("close");
	button->callback = [this] { set_destroyed(); };
	button->set_position({0.0f, -100.0f});
	push(std::move(button));
}

Prefs::View::~View() { m_prefs.save(*m_app); }
} // namespace spaced
