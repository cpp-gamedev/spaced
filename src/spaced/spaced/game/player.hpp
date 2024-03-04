#pragma once
#include <bave/graphics/sprite_anim.hpp>
#include <bave/logger.hpp>
#include <spaced/game/health.hpp>
#include <spaced/game/player_controller.hpp>

namespace spaced {
class Player : public bave::IDrawable {
  public:
	using Controller = PlayerController;

	explicit Player(Services const& services);

	void on_move(bave::PointerMove const& pointer_move);
	void on_tap(bave::PointerTap const& pointer_tap);

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const final;

	void debug_stuff();

	void set_y(float y);
	[[nodiscard]] auto get_y() const -> float { return ship.transform.position.y; }

	bave::SpriteAnim ship{};
	Health health{};

  private:
	bave::Logger m_log{"Player"};
	Controller m_controller;
};
} // namespace spaced
