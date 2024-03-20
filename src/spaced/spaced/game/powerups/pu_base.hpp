#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/shape.hpp>
#include <spaced/game/powerup.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>

namespace spaced {
class PUBase : public IPowerup {
  public:
	explicit PUBase(Services const& services, std::string_view name);

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const;

	[[nodiscard]] auto get_bounds() const -> bave::Rect<> final { return shape.get_bounds(); }
	void activate(Player& player) final;

	[[nodiscard]] auto is_destroyed() const -> bool { return m_destroyed; }

	float speed{100.0f};
	bave::CircleShape shape{};

  protected:
	virtual void do_activate(Player& player) = 0;

	bave::NotNull<Services const*> m_services;
	bave::NotNull<ILayout const*> m_layout;
	std::string_view m_name{};
	bool m_destroyed{};
};
} // namespace spaced
