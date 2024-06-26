#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/particle_system.hpp>
#include <bave/graphics/shape.hpp>
#include <bave/services/services.hpp>
#include <spaced/services/layout.hpp>

namespace spaced {
class Player;

class Powerup : public bave::IDrawable {
  public:
	explicit Powerup(bave::Services const& services, std::string_view name);

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const final;

	[[nodiscard]] auto get_bounds() const -> bave::Rect<> { return shape.get_bounds(); }
	void activate(Player& player);

	[[nodiscard]] auto is_destroyed() const -> bool { return m_destroyed; }

	float speed{300.0f};
	bave::CircleShape shape{};
	bave::ParticleEmitter emitter{};

  protected:
	virtual void do_activate(Player& player) = 0;

	bave::NotNull<bave::Services const*> m_services;
	bave::NotNull<Layout const*> m_layout;
	std::string_view m_name{};
	bool m_emitter_ticked{};
	bool m_destroyed{};
};
} // namespace spaced
