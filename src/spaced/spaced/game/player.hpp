#pragma once
#include <bave/graphics/particle_system.hpp>
#include <bave/graphics/shape.hpp>
#include <bave/logger.hpp>
#include <spaced/game/controller.hpp>
#include <spaced/game/health.hpp>
#include <spaced/game/weapon.hpp>
#include <spaced/game/world_spec.hpp>

namespace spaced {
class Player : public bave::IDrawable {
  public:
	explicit Player(Services const& services, std::unique_ptr<IController> controller);

	void on_focus(bave::FocusChange const& focus_change);
	void on_move(bave::PointerMove const& pointer_move);
	void on_tap(bave::PointerTap const& pointer_tap);

	void tick(std::span<bave::NotNull<IDamageable*> const> targets, bave::Seconds dt);
	void draw(bave::Shader& shader) const final;

	void setup(WorldSpec::Player const& spec);

	void set_y(float y);
	[[nodiscard]] auto get_y() const -> float { return ship.transform.position.y; }

	[[nodiscard]] auto get_muzzle_position() const -> glm::vec2;
	[[nodiscard]] auto get_exhaust_position() const -> glm::vec2;

	void set_controller(std::unique_ptr<IController> controller);
	[[nodiscard]] auto get_controller() const -> IController const& { return *m_controller; }

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	bave::RoundedQuadShape ship{};
	Health health{};

  private:
	void setup_ship();

	void do_inspect();
	void debug_switch_weapon();

	bave::Logger m_log{"Player"};
	bave::NotNull<Services const*> m_services;
	std::unique_ptr<IController> m_controller;
	bave::ParticleEmitter m_exhaust{};
	std::unique_ptr<Weapon> m_weapon{};
	std::vector<std::unique_ptr<Weapon::Round>> m_weapon_rounds{};

	struct {
		int shots_remaining{};
	} m_debug{};
};
} // namespace spaced
