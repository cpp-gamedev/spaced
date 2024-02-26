#include <spaced/services/resources.hpp>
#include <spaced/ui/text.hpp>

namespace spaced::ui {
Text::Text(Services const& services) {
	text.set_font(services.get<Resources>().main_font);
	text.tint = bave::white_v;
}
} // namespace spaced::ui
