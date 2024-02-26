#pragma once
#include <bave/core/string_hash.hpp>
#include <spaced/services/service.hpp>
#include <spaced/ui/style.hpp>
#include <unordered_map>

namespace dj {
class Json;
}

namespace spaced::ui {
struct Styles : IService {
	template <typename Type>
	using Map = std::unordered_map<std::string, Type, bave::StringHash, std::equal_to<>>;

	Map<ButtonStyle> buttons{};
	Map<ProgressBarStyle> progress_bars{};
	CollectListItemStyle collect_list_item{};
	LoadingScreenStyle loading_screen{};

	static auto load(dj::Json const& json) -> Styles;

	[[nodiscard]] auto get_button(std::string_view id) const -> ButtonStyle const&;
	[[nodiscard]] auto get_progress_bar(std::string_view id) const -> ProgressBarStyle const&;

	[[nodiscard]] auto save() const -> dj::Json;
};
} // namespace spaced::ui
