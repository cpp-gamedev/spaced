#include <spaced/util.hpp>

namespace spaced {
auto util::create_font_atlas_task(std::shared_ptr<bave::Font> font, std::vector<bave::TextHeight> heights) -> std::function<void()> {
	if (!font || heights.empty()) { return {}; }
	return [font = std::move(font), heights = std::move(heights)] {
		for (auto const height : heights) { font->create_font_atlas(height); }
	};
}
} // namespace spaced
