#include <spaced/ui/view.hpp>

namespace spaced::ui {
using bave::PointerMove;
using bave::PointerTap;
using bave::Seconds;
using bave::Shader;

View::View(Services const& services) : m_layout(&services.get<ILayout>()) {}

void View::push(std::unique_ptr<IElement> element) {
	if (!element) { return; }
	m_elements.push_back(std::move(element));
	if (auto* widget = dynamic_cast<IWidget*>(m_elements.back().get())) { m_widgets.push_back(widget); }
}

void View::on_move(PointerMove const& pointer_move) {
	for (auto const& widget : m_widgets) { widget->on_move(pointer_move); }
}

void View::on_tap(PointerTap const& pointer_tap) {
	for (auto const& widget : m_widgets) { widget->on_tap(pointer_tap); }
}

void View::tick(Seconds const dt) {
	for (auto const& widget : m_widgets) { widget->tick(dt); }
}

void View::render(Shader& shader) const {
	if (m_widgets.empty()) { return; }
	shader.set_render_view(m_layout->get_main_view());
	for (auto const& element : m_elements) { element->draw(shader); }
}
} // namespace spaced::ui
