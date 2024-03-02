#pragma once
#include <bave/graphics/sprite_anim.hpp>
#include <bave/logger.hpp>
#include <spaced/game/player_controller.hpp>
#include <spaced/game/weapon.hpp>

namespace spaced {
class Player : public bave::IDrawable {
  public:
	using Controller = PlayerController;

	explicit Player(Services const& services);

	void on_focus(bave::FocusChange const& focus_change);
	void on_move(bave::PointerMove const& pointer_move);
	void on_tap(bave::PointerTap const& pointer_tap);

	void tick(std::span<bave::NotNull<IDamageable*> const> targets, bave::Seconds dt);
	void draw(bave::Shader& shader) const final;

	void debug_stuff();

	void set_y(float y);
	[[nodiscard]] auto get_y() const -> float { return ship.transform.position.y; }

	bave::SpriteAnim ship{};

  private:
	void debug_switch_weapon();

	bave::Logger m_log{"Player"};
	bave::NotNull<Services const*> m_services;
	Controller m_controller;
	std::unique_ptr<Weapon> m_weapon{};
	std::vector<std::unique_ptr<Weapon::Round>> m_weapon_rounds{};

	struct {
		int shots_remaining{};
	} m_debug{};
};
} // namespace spaced
