#pragma once
#include <bave/services/styles.hpp>
#include <bave/ui/outline_quad.hpp>
#include <bave/ui/sprite.hpp>
#include <bave/ui/text.hpp>
#include <bave/ui/view.hpp>

namespace spaced {
struct Layout;

class Hud : public bave::ui::View {
  public:
	explicit Hud(bave::Services const& services);

	void set_lives(int lives);
	void set_score(std::int64_t score);
	void set_hi_score(std::int64_t score);
	void set_weapon(std::shared_ptr<bave::Texture const> texture);
	void set_rounds(int count);

  private:
	[[nodiscard]] auto make_text(bave::Services const& services) const -> std::unique_ptr<bave::ui::Text>;

	void create_background();
	void create_score(bave::Services const& services);
	void create_lives(bave::Services const& services);
	void create_weapon(bave::Services const& services);

	bave::NotNull<Layout const*> m_layout;
	bave::NotNull<bave::Styles const*> m_styles;
	glm::vec2 m_text_bounds_size{};

	bave::Ptr<bave::ui::OutlineQuad> m_background{};
	bave::Ptr<bave::ui::Text> m_score{};
	bave::Ptr<bave::ui::Text> m_hi_score{};

	bave::Ptr<bave::ui::Sprite> m_lives_icon{};
	bave::Ptr<bave::ui::Text> m_lives_count{};

	bave::Ptr<bave::ui::Sprite> m_weapon_icon{};
	bave::Ptr<bave::ui::Text> m_round_count{};
};
} // namespace spaced
