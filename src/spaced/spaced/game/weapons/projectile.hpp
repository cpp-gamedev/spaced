#pragma once
#include <bave/graphics/shape.hpp>
#include <spaced/game/weapon_round.hpp>
#include <spaced/services/layout.hpp>

namespace spaced {
class Projectile : public IWeaponRound {
  public:
	struct Config {
		std::shared_ptr<bave::Texture> texture{};
		glm::vec2 size{60.0f, 5.0f};
		float corner_radius{2.5f};
		float x_speed{1500.0f};
		bave::Rgba tint{bave::white_v};
		float damage{1.0f};
	};

	explicit Projectile(bave::NotNull<ILayout const*> layout, Config config, glm::vec2 muzzle_position);

	void tick(State const& state, bave::Seconds dt) override;
	[[nodiscard]] auto is_destroyed() const -> bool override { return m_destroyed; }
	void draw(bave::Shader& shader) const override;

  protected:
	bave::NotNull<ILayout const*> m_layout;
	Config m_config{};

	bave::RoundedQuadShape m_shape{};
	bool m_destroyed{};
};
} // namespace spaced
