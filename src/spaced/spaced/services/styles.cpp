#include <bave/io/json_io.hpp>
#include <spaced/services/styles.hpp>
#include <spaced/util.hpp>

namespace spaced {
namespace {
using bave::from_json;
using bave::to_json;
using util::from_json;
using util::to_json;

template <typename Type>
void load_styles(StringMap<Type>& out, dj::Json const& json) {
	for (auto const& [id, in_style] : json.object_view()) {
		if (id.empty()) { continue; }
		auto out_style = Type{};
		from_json(in_style, out_style);
		out.insert_or_assign(std::string{id}, out_style);
	}
}

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
	load_styles(ret.rgbas, json["rgbas"]);
	load_styles(ret.buttons, json["buttons"]);
	load_styles(ret.progress_bars, json["progress_bars"]);
	load_styles(ret.dialogs, json["dialogs"]);
	load_styles(ret.sliders, json["sliders"]);
	if (auto const& loading_screen = json["loading_screen"]) { from_json(loading_screen, ret.loading_screen); }
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
