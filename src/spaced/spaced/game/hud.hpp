#pragma once
#include <spaced/services/styles.hpp>
#include <spaced/ui/outline_quad.hpp>
#include <spaced/ui/text.hpp>
#include <spaced/ui/view.hpp>

namespace spaced {
class Hud : public ui::View {
  public:
	explicit Hud(Services const& services);

	void set_score(std::int64_t score);

  private:
	void create_background();
	void create_score(Services const& services);

	bave::NotNull<Styles const*> m_styles;
	bave::Rect<> m_area{};

	bave::Ptr<ui::OutlineQuad> m_background{};
	bave::Ptr<ui::Text> m_score{};
};
} // namespace spaced
