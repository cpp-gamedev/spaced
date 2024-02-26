#include <spaced/services/styles.hpp>
#include <spaced/util.hpp>

namespace spaced::ui {
namespace {
using util::from_json;
using util::to_json;

template <typename Type>
void load_styles(Styles::Map<Type>& out, dj::Json const& json) {
	for (auto const& [id, in_style] : json.object_view()) {
		if (id.empty()) { continue; }
		auto out_style = Type{};
		from_json(in_style, out_style);
		out.insert_or_assign(std::string{id}, out_style);
	}
}

template <typename Type>
void save_styles(Styles::Map<Type> const& styles, dj::Json& out, std::string_view const key) {
	auto out_styles = dj::Json{};
	for (auto const& [id, button_style] : styles) {
		if (id.empty()) { continue; }
		to_json(out_styles[id], button_style);
	}
	if (out_styles) { out[key] = std::move(out_styles); }
}

template <typename Type>
[[nodiscard]] auto get_or_default(Styles::Map<Type> const& map, std::string_view const id) -> Type const& {
	if (auto const it = map.find(id); it != map.end()) { return it->second; }
	static auto const fallback = Type{};
	return fallback;
}
} // namespace

auto Styles::load(dj::Json const& json) -> Styles {
	auto ret = Styles{};
	load_styles(ret.buttons, json["buttons"]);
	load_styles(ret.progress_bars, json["progress_bars"]);
	if (auto const& loading_screen = json["loading_screen"]) { from_json(loading_screen, ret.loading_screen); }
	if (auto const& collect_list_item = json["collect_list_item"]) { from_json(collect_list_item, ret.collect_list_item); }
	return ret;
}

auto Styles::get_button(std::string_view id) const -> ButtonStyle const& { return get_or_default(buttons, id); }
auto Styles::get_progress_bar(std::string_view id) const -> ProgressBarStyle const& { return get_or_default(progress_bars, id); }

auto Styles::save() const -> dj::Json {
	auto ret = dj::Json{};
	save_styles(buttons, ret, "buttons");
	save_styles(progress_bars, ret, "progress_bars");
	to_json(ret["loading_screen"], loading_screen);
	to_json(ret["collect_list_item"], collect_list_item);
	return ret;
}
} // namespace spaced::ui
