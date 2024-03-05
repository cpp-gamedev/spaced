#pragma once
#include <bave/graphics/sprite_anim.hpp>
#include <bave/platform.hpp>
#include <spaced/game/damageable.hpp>
#include <spaced/game/health.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>

namespace spaced {
class Enemy : public IDamageable, public bave::IDrawable {
  public:
	explicit Enemy(Services const& services, std::string_view type);

	[[nodiscard]] auto get_bounds() const -> bave::Rect<> override { return sprite.get_bounds(); }
	auto take_damage(float damage) -> bool override;

	[[nodiscard]] auto is_destroyed() const -> bool { return health.is_dead(); }

	virtual void tick(bave::Seconds dt) = 0;
	void draw(bave::Shader& shader) const override { sprite.draw(shader); }

	void setup(glm::vec2 max_size, float y_position);

	[[nodiscard]] auto get_layout() const -> ILayout const& { return *m_layout; }

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	bave::SpriteAnim sprite{};
	Health health{};

  private:
	virtual void do_inspect();

	bave::Ptr<ILayout const> m_layout{};
	std::string_view m_type{};
};
} // namespace spaced
