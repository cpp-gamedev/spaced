#pragma once
#include <bave/font/font.hpp>
#include <functional>

namespace spaced::util {
auto create_font_atlas_task(std::shared_ptr<bave::Font> font, std::vector<bave::TextHeight> heights) -> std::function<void()>;
} // namespace spaced::util
