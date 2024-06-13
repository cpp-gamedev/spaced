#include <bave/io/json_io.hpp>
#include <spaced/services/styles.hpp>
#include <spaced/util.hpp>

namespace spaced {
namespace {
using bave::from_json;
using bave::Rgba;

class StylesLoader {
  public:
	StringMap<Rgba> const& rgbas; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

	template <typename Type>
	void load(StringMap<Type>& out, dj::Json const& json) const {
		for (auto const& [id, in_style] : json.object_view()) {
			if (id.empty()) { continue; }
			auto out_style = Type{};
			from(in_style, out_style);
			out.insert_or_assign(std::string{id}, out_style);
		}
	}

	template <typename Type>
	void from(dj::Json const& json, Type& out) const {
		from_json(json, out);
	}

	void from(dj::Json const& json, Rgba& out) const {
		auto const str = json.as_string();
		if (str.starts_with('#')) {
			from_json(json, out);
		} else {
			out = rgbas[str];
		}
	}

	void from(dj::Json const& json, ui::LoadingScreenStyle& out) const {
		from(json["background_tint"], out.background_tint);
		auto const& spinner = json["spinner"];
		from(spinner["size"], out.spinner.size);
		from(spinner["spin_rate"], out.spinner.spin_rate.value);
		auto const& progress_bar = json["progress_bar"];
		from(progress_bar["n_width"], out.progress_bar.n_width);
		from(progress_bar["height"], out.progress_bar.height);
		from(progress_bar["bottom_offset"], out.progress_bar.bottom_offset);
		from(progress_bar["padding"], out.progress_bar.padding);
	}

	void from(dj::Json const& json, ui::ButtonStyle::Tint& out) const {
		from(json["idle"], out[ui::ButtonState::eIdle]);
		from(json["hover"], out[ui::ButtonState::eHover]);
		from(json["press"], out[ui::ButtonState::ePress]);
	}

	void from(dj::Json const& json, ui::ButtonStyle& out) const {
		from(json["background"], out.background);
		from(json["outline"], out.outline);
		from(json["text"], out.text);
		from(json["outline_width"], out.outline_width);
		from(json["corner_ratio"], out.corner_ratio);
	}

	void from(dj::Json const& json, ui::ProgressBarStyle& out) const {
		from(json["background"], out.background);
		from(json["fill"], out.fill);
		from(json["corner_ratio"], out.corner_ratio);
		from(json["padding"], out.padding);
	}

	void from(dj::Json const& json, ui::DialogStyle& out) const {
		from(json["footer_padding"], out.footer_padding);
		from(json["corner_ratio"], out.corner_ratio);
		from(json["background_tint"], out.background_tint);
		from(json["outline_tint"], out.outline_tint);
		from(json["content_text_tint"], out.content_text_tint);
	}

	void from(dj::Json const& json, ui::SliderStyle& out) const {
		from(json["progress_bar"], out.progress_bar);
		from(json["knob_diameter"], out.knob_diameter);
		from(json["knob_tint"], out.knob_tint);
	}
};
} // namespace

namespace {
using bave::to_json;
using util::to_json;

template <typename Type>
void save_styles(StringMap<Type> const& styles, dj::Json& out, std::string_view const key) {
	auto out_styles = dj::Json{};
	for (auto const& [id, button_style] : styles) {
		if (id.empty()) { continue; }
		to_json(out_styles[id], button_style);
	}
	if (out_styles) { out[key] = std::move(out_styles); }
}
} // namespace

auto Styles::load(dj::Json const& json) -> Styles {
	auto ret = Styles{};
	auto const loader = StylesLoader{ret.rgbas};
	loader.load(ret.rgbas, json["rgbas"]);
	loader.load(ret.buttons, json["buttons"]);
	loader.load(ret.progress_bars, json["progress_bars"]);
	loader.load(ret.dialogs, json["dialogs"]);
	loader.load(ret.sliders, json["sliders"]);
	if (auto const& loading_screen = json["loading_screen"]) { loader.from(loading_screen, ret.loading_screen); }
	return ret;
}

auto Styles::save() const -> dj::Json {
	auto ret = dj::Json{};
	save_styles(rgbas, ret, "rgbas");
	save_styles(buttons, ret, "buttons");
	save_styles(progress_bars, ret, "progress_bars");
	save_styles(dialogs, ret, "dialogs");
	save_styles(sliders, ret, "sliders");
	to_json(ret["loading_screen"], loading_screen);
	return ret;
}
} // namespace spaced
