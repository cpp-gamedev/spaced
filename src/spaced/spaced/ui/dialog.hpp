#pragma once
#include <spaced/ui/button.hpp>
#include <spaced/ui/outline_quad.hpp>
#include <spaced/ui/text.hpp>
#include <spaced/ui/view.hpp>

namespace spaced::ui {
struct DialogCreateInfo {
	struct ButtonInfo {
		std::string text{};
		std::function<void()> callback{};
	};

	glm::vec2 size{500.0f, 200.0f};
	std::string content_text{"Content text."};
	ButtonInfo main_button{.text = "OK"};
	ButtonInfo second_button{};
};

class Dialog : public ui::View {
  public:
	using CreateInfo = DialogCreateInfo;

	static constexpr auto text_height_v = bave::TextHeight{static_cast<int>(bave::TextHeight::eDefault) + 16};

	explicit Dialog(Services const& services, CreateInfo create_info = {});

  protected:
	DialogStyle m_style{};

	bave::Ptr<OutlineQuad> m_background{};
	bave::Ptr<Text> m_content_text{};
	std::array<bave::Ptr<ui::Button>, 2> m_footer_buttons{};
};
} // namespace spaced::ui
