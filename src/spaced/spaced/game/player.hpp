#pragma once
#include <bave/graphics/particle_system.hpp>
#include <bave/graphics/shape.hpp>
#include <bave/logger.hpp>
#include <spaced/game/arsenal.hpp>
#include <spaced/game/controller.hpp>
#include <spaced/game/health.hpp>
#include <spaced/game/powerup.hpp>
#include <spaced/game/shield.hpp>

namespace spaced {
struct Stats;
struct Sig1up;

class Player : public IDamageable, public bave::IDrawable {
  public:
	struct State {
		std::span<bave::NotNull<IDamageable*> const> targets{};
		std::span<bave::NotNull<Powerup*> const> powerups{};
	};

	explicit Player(bave::Services const& services, std::unique_ptr<IController> controller);

	void on_focus(bave::FocusChange const& focus_change);
	void on_move(bave::PointerMove const& pointer_move);
	void on_tap(bave::PointerTap const& pointer_tap);

	void tick(State const& state, bave::Seconds dt);
	void draw(bave::Shader& shader) const final;

	[[nodiscard]] auto get_instigator() const -> Instigator final { return Instigator::ePlayer; }
	[[nodiscard]] auto get_bounds() const -> bave::Rect<> final;
	auto take_damage(float damage) -> bool final;
	void force_death() final;

	void set_y(float y);
	[[nodiscard]] auto get_y() const -> float { return ship.transform.position.y; }

	[[nodiscard]] auto get_muzzle_position() const -> glm::vec2;
	[[nodiscard]] auto get_exhaust_position() const -> glm::vec2;

	void set_controller(std::unique_ptr<IController> controller);
	[[nodiscard]] auto get_controller() const -> IController const& { return *m_controller; }

	void set_special_weapon(std::unique_ptr<Weapon> weapon) { m_arsenal.set_special(std::move(weapon)); }
	[[nodiscard]] auto get_weapon() const -> Weapon const& { return m_arsenal.get_weapon(); }

	void set_shield(bave::Seconds ttl);
	void one_up();

	[[nodiscard]] auto is_dead() const -> bool { return m_health.is_dead(); }
	[[nodiscard]] auto is_idle() const -> bool { return m_exhaust.active_particles() == 0; }

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	bave::Sprite ship{};
	glm::vec2 hitbox_size{75.0f};

  private:
	void on_death(bave::Seconds dt);
	auto check_hit(IDamageable& out, bave::Rect<> const& hitbox, bave::Seconds dt) -> bool;

	void do_inspect();

	bave::Logger m_log{"Player"};
	bave::NotNull<bave::Services const*> m_services;
	bave::NotNull<bave::IAudio*> m_audio;
	bave::NotNull<Stats*> m_stats;
	std::unique_ptr<IController> m_controller;
	bave::NotNull<Sig1up*> m_on_1up;

	bave::ParticleEmitter m_exhaust{};
	Shield m_shield;

	float m_heat_being_rendered{};
	float m_heat{};
	float m_heat_increment{0.1f};
	float m_heat_dissipated{0.4f}; // per second
	bave::Rgba m_heat_color{};
	bool m_is_cooling_down{};
	float m_cooldown_threshold{0.5f};

	bave::ParticleEmitter m_death_source{};
	std::optional<bave::ParticleEmitter> m_death{};

	Arsenal m_arsenal;
	Health m_health{};
};
} // namespace spaced
