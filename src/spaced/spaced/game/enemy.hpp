#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/sprite.hpp>
#include <bave/platform.hpp>
#include <bave/services/services.hpp>
#include <bave/ui/progress_bar.hpp>
#include <spaced/game/damageable.hpp>
#include <spaced/game/health.hpp>
#include <spaced/services/layout.hpp>

namespace spaced {
class Enemy : public IDamageable, public bave::IDrawable {
  public:
	explicit Enemy(bave::Services const& services, std::string_view type);

	[[nodiscard]] auto get_instigator() const -> Instigator final { return Instigator::eEnemy; }
	[[nodiscard]] auto get_bounds() const -> bave::Rect<> override;
	[[nodiscard]] auto get_position() const -> glm::vec2 { return m_sprite.transform.position; }

	auto take_damage(float damage) -> bool override;
	void force_death() override;

	[[nodiscard]] auto is_dead() const -> bool { return health.is_dead(); }
	[[nodiscard]] auto is_destroyed() const -> bool { return is_dead() || m_destroyed; }
	void set_destroyed() { m_destroyed = true; }

	virtual void tick(bave::Seconds dt, bool in_play) = 0;
	void draw(bave::Shader& shader) const override;

	void setup(glm::vec2 max_size, float y_position);
	void update_health_bar();

	[[nodiscard]] auto get_layout() const -> Layout const& { return *m_layout; }

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	Health health{};
	std::int64_t points{10};

	std::string death_emitter{"particles/explode.json"};
	std::vector<std::string> death_sfx{"sfx/swish.wav"};

  protected:
	bave::Sprite m_sprite{};
	std::optional<glm::vec2> m_hitbox{};

	virtual void do_inspect();

  private:
	bave::NotNull<Layout const*> m_layout;

	bave::ui::ProgressBar m_health_bar;

	std::string_view m_type{};
	bool m_destroyed{};
};
} // namespace spaced
