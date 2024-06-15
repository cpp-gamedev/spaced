#pragma once
#include <bave/services/styles.hpp>
#include <bave/ui/outline_quad.hpp>
#include <bave/ui/text.hpp>
#include <bave/ui/view.hpp>

namespace spaced {
struct Layout;

class Hud : public bave::ui::View {
  public:
	explicit Hud(bave::Services const& services);

	void set_score(std::int64_t score);
	void set_hi_score(std::int64_t score);

  private:
	void create_background();
	void create_score(bave::Services const& services);

	bave::NotNull<bave::IDisplay const*> m_display;
	bave::NotNull<Layout const*> m_layout;
	bave::NotNull<bave::Styles const*> m_styles;
	glm::vec2 m_text_bounds_size{};

	bave::Ptr<bave::ui::OutlineQuad> m_background{};
	bave::Ptr<bave::ui::Text> m_score{};
	bave::Ptr<bave::ui::Text> m_hi_score{};
};
} // namespace spaced
