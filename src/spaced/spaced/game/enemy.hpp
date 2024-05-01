#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/shape.hpp>
#include <bave/platform.hpp>
#include <spaced/game/damageable.hpp>
#include <spaced/game/health.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>
#include <spaced/ui/progress_bar.hpp>

namespace spaced {
class Enemy : public IDamageable, public bave::IDrawable {
  public:
	explicit Enemy(Services const& services, std::string_view type);

	[[nodiscard]] auto get_bounds() const -> bave::Rect<> override { return shape.get_bounds(); }
	auto take_damage(float damage) -> bool override;
	void force_death() override;

	[[nodiscard]] auto is_dead() const -> bool { return health.is_dead(); }
	[[nodiscard]] auto is_destroyed() const -> bool { return is_dead() || m_destroyed; }
	void set_destroyed() { m_destroyed = true; }

	virtual void tick(bave::Seconds dt, bool in_play);
	void draw(bave::Shader& shader) const override;

	void setup(glm::vec2 max_size, float y_position);

	[[nodiscard]] auto get_layout() const -> ILayout const& { return *m_layout; }

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	bave::RoundedQuadShape shape{};
	Health health{};
	std::int64_t points{10};

	std::string death_emitter{"particles/explode.json"};
	std::vector<std::string> death_sfx{"sfx/bubble.wav"};

  private:
	virtual void do_inspect();

	bave::NotNull<ILayout const*> m_layout;

	ui::ProgressBar m_health_bar;

	std::string_view m_type{};
	bool m_destroyed{};
};
} // namespace spaced
