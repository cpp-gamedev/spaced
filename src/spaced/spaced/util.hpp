#pragma once
#include <bave/font/font.hpp>
#include <djson/json.hpp>
#include <spaced/ui/style.hpp>
#include <functional>

namespace spaced::util {
void to_json(dj::Json& out, ui::LoadingScreenStyle const& loading_screen_style);
void from_json(dj::Json const& json, ui::LoadingScreenStyle& out);

void to_json(dj::Json& out, ui::ButtonStyle::Tint const& button_tint);
void from_json(dj::Json const& json, ui::ButtonStyle::Tint& out);

void to_json(dj::Json& out, ui::ButtonStyle const& button_style);
void from_json(dj::Json const& json, ui::ButtonStyle& out);

void to_json(dj::Json& out, ui::ProgressBarStyle const& progress_bar_style);
void from_json(dj::Json const& json, ui::ProgressBarStyle& out);

auto create_font_atlas_task(std::shared_ptr<bave::Font> font, std::vector<bave::TextHeight> heights) -> std::function<void()>;
} // namespace spaced::util
