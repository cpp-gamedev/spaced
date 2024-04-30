#include <spaced/services/styles.hpp>
#include <spaced/ui/dialog.hpp>

namespace spaced::ui {
Dialog::Dialog(Services const& services, CreateInfo create_info) : View(services), m_style(services.get<Styles>().dialogs["default"]) {
	auto main_button = std::make_unique<Button>(services);
	auto const footer_height = main_button->get_size().y + m_style.footer_padding.y;
	auto const footer_position = glm::vec2{0.0f, -0.5f * (create_info.size.y - footer_height)};

	auto background = std::make_unique<OutlineQuad>();
	background->set_size({create_info.size.x, create_info.size.y + footer_height});
	background->set_tint(m_style.background_tint);
	background->set_outline_tint(m_style.outline_tint);
	push(std::move(background));

	auto content_text = std::make_unique<Text>(services);
	content_text->text.set_string(std::move(create_info.content_text));
	content_text->text.set_height(text_height_v);
	content_text->text.tint = m_style.content_text_tint;
	push(std::move(content_text));

	auto const button_x_offset = create_info.second_button.text.empty() ? 0.0f : 0.25f * create_info.size.x;
	main_button->set_text(std::move(create_info.main_button.text));
	main_button->callback = [this, f = std::move(create_info.main_button.callback)] {
		if (f) { f(); }
		set_destroyed();
	};
	if (create_info.second_button.text.empty()) {
		main_button->set_position(footer_position);
	} else {
		auto const x_offset = -0.5f * (create_info.size.x - main_button->get_size().x) + m_style.footer_padding.x;
		main_button->set_position({footer_position.x + x_offset, footer_position.y});
	}
	push(std::move(main_button));

	if (!create_info.second_button.text.empty()) {
		auto second_button = std::make_unique<Button>(services);
		second_button->set_text(std::move(create_info.second_button.text));
		second_button->callback = [this, f = std::move(create_info.second_button.callback)] {
			if (f) { f(); }
			set_destroyed();
		};
		auto const x_offset = 0.5f * (create_info.size.x - second_button->get_size().x) - m_style.footer_padding.x;
		second_button->set_position({footer_position.x + x_offset, footer_position.y});
		push(std::move(second_button));
	}
}
} // namespace spaced::ui
