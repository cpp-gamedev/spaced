#include <bave/io/json_io.hpp>
#include <spaced/util.hpp>

namespace spaced {
void util::to_json(dj::Json& out, ui::LoadingScreenStyle const& loading_screen_style) {
	using bave::to_json;
	to_json(out["background_tint"], loading_screen_style.background_tint);
	auto& spinner = out["spinner"];
	to_json(spinner["size"], loading_screen_style.spinner.size);
	spinner["spin_rate"] = loading_screen_style.spinner.spin_rate.value;
	auto& progress_bar = out["progress_bar"];
	progress_bar["n_width"] = loading_screen_style.progress_bar.n_width;
	progress_bar["height"] = loading_screen_style.progress_bar.height;
	progress_bar["bottom_offset"] = loading_screen_style.progress_bar.bottom_offset;
	progress_bar["padding"] = loading_screen_style.progress_bar.padding;
}

void util::from_json(dj::Json const& json, ui::LoadingScreenStyle& out) {
	using bave::from_json;
	from_json(json["background_tint"], out.background_tint);
	auto const& spinner = json["spinner"];
	from_json(spinner["size"], out.spinner.size);
	out.spinner.spin_rate.value = spinner["spin_rate"].as<float>(out.spinner.spin_rate.value);
	auto const& progress_bar = json["progress_bar"];
	out.progress_bar.n_width = progress_bar["n_width"].as<float>(out.progress_bar.n_width);
	out.progress_bar.height = progress_bar["height"].as<float>(out.progress_bar.height);
	out.progress_bar.bottom_offset = progress_bar["bottom_offset"].as<float>(out.progress_bar.bottom_offset);
	out.progress_bar.padding = progress_bar["padding"].as<float>(out.progress_bar.padding);
}

void util::to_json(dj::Json& out, ui::ButtonStyle::Tint const& button_tint) {
	using bave::to_json;
	to_json(out["idle"], button_tint[ui::ButtonState::eIdle]);
	to_json(out["hover"], button_tint[ui::ButtonState::eHover]);
	to_json(out["press"], button_tint[ui::ButtonState::ePress]);
}

void util::from_json(dj::Json const& json, ui::ButtonStyle::Tint& out) {
	from_json(json["idle"], out[ui::ButtonState::eIdle]);
	from_json(json["hover"], out[ui::ButtonState::eHover]);
	from_json(json["press"], out[ui::ButtonState::ePress]);
}

void util::to_json(dj::Json& out, ui::ButtonStyle const& button_style) {
	to_json(out["background"], button_style.background);
	to_json(out["outline"], button_style.outline);
	to_json(out["text"], button_style.text);
	out["outline_width"] = button_style.outline_width;
	out["corner_ratio"] = button_style.corner_ratio;
}

void util::from_json(dj::Json const& json, ui::ButtonStyle& out) {
	from_json(json["background"], out.background);
	from_json(json["outline"], out.outline);
	from_json(json["text"], out.text);
	out.outline_width = json["outline_width"].as<float>();
	out.corner_ratio = json["corner_ratio"].as<float>();
}

void util::to_json(dj::Json& out, ui::ProgressBarStyle const& progress_bar_style) {
	to_json(out["background"], progress_bar_style.background);
	to_json(out["fill"], progress_bar_style.fill);
	out["corner_ratio"] = progress_bar_style.corner_ratio;
	out["padding"] = progress_bar_style.padding;
}

void util::from_json(dj::Json const& json, ui::ProgressBarStyle& out) {
	from_json(json["background"], out.background);
	from_json(json["fill"], out.fill);
	out.corner_ratio = json["corner_ratio"].as<float>(out.corner_ratio);
	out.padding = json["padding"].as<float>(out.padding);
}

void util::to_json(dj::Json& out, ui::DialogStyle const& dialog_style) {
	using bave::to_json;
	to_json(out["footer_padding"], dialog_style.footer_padding);
	to_json(out["corner_ratio"], dialog_style.corner_ratio);
	to_json(out["background_tint"], dialog_style.background_tint);
	to_json(out["outline_tint"], dialog_style.outline_tint);
	to_json(out["content_text_tint"], dialog_style.content_text_tint);
}

void util::from_json(dj::Json const& json, ui::DialogStyle& out) {
	using bave::from_json;
	from_json(json["footer_padding"], out.footer_padding);
	from_json(json["corner_ratio"], out.corner_ratio);
	from_json(json["background_tint"], out.background_tint);
	from_json(json["outline_tint"], out.outline_tint);
	from_json(json["content_text_tint"], out.content_text_tint);
}

auto util::create_font_atlas_task(std::shared_ptr<bave::Font> font, std::vector<bave::TextHeight> heights) -> std::function<void()> {
	if (!font || heights.empty()) { return {}; }
	return [font = std::move(font), heights = std::move(heights)] {
		for (auto const height : heights) { font->create_font_atlas(height); }
	};
}
} // namespace spaced
