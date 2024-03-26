#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/shape.hpp>
#include <bave/platform.hpp>
#include <spaced/game/damageable.hpp>
#include <spaced/game/enemy_death.hpp>
#include <spaced/game/health.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>

namespace spaced {
class Enemy : public IDamageable, public bave::IDrawable {
  public:
	explicit Enemy(Services const& services, bave::NotNull<IEnemyDeathListener*> listener, std::string_view type);

	[[nodiscard]] auto get_bounds() const -> bave::Rect<> override { return shape.get_bounds(); }
	auto take_damage(float damage) -> bool override;

	[[nodiscard]] auto is_dead() const -> bool { return health.is_dead(); }
	[[nodiscard]] auto is_destroyed() const -> bool { return is_dead() || m_destroyed; }
	void set_destroyed() { m_destroyed = true; }

	virtual void tick(bave::Seconds dt) = 0;
	void draw(bave::Shader& shader) const override { shape.draw(shader); }

	void setup(glm::vec2 max_size, float y_position);

	[[nodiscard]] auto get_layout() const -> ILayout const& { return *m_layout; }
	[[nodiscard]] auto get_death_listener() const -> IEnemyDeathListener& { return *m_listener; }

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	bave::RoundedQuadShape shape{};
	Health health{};
	std::int64_t points{10};

  private:
	virtual void do_inspect();

	bave::NotNull<ILayout const*> m_layout;
	bave::NotNull<IEnemyDeathListener*> m_listener;
	std::string_view m_type{};
	bool m_destroyed{};
};
} // namespace spaced
