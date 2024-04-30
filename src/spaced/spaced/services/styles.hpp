#pragma once
#include <spaced/services/service.hpp>
#include <spaced/string_map.hpp>
#include <spaced/ui/style.hpp>

namespace dj {
class Json;
}

namespace spaced {
struct Styles : IService {
	StringMap<bave::Rgba> rgbas{};
	StringMap<ui::ButtonStyle> buttons{};
	StringMap<ui::ProgressBarStyle> progress_bars{};
	StringMap<ui::DialogStyle> dialogs{};
	ui::LoadingScreenStyle loading_screen{};

	static auto load(dj::Json const& json) -> Styles;

	[[nodiscard]] auto save() const -> dj::Json;
};
} // namespace spaced
