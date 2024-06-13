#pragma once
#include <bave/graphics/shape.hpp>
#include <bave/services/display.hpp>
#include <spaced/game/instigator.hpp>
#include <spaced/game/weapon_round.hpp>

namespace spaced {
class Projectile : public IWeaponRound {
  public:
	struct Config {
		std::shared_ptr<bave::Texture> texture{};
		glm::vec2 size{60.0f, 10.0f};
		float corner_radius{0.5f * size.y};
		float x_speed{1500.0f};
		bave::Rgba tint{bave::black_v};
		float damage{1.0f};
		Instigator instigator{Instigator::ePlayer};
	};

	explicit Projectile(bave::NotNull<bave::IDisplay const*> display, Config config, glm::vec2 muzzle_position);

	void tick(State const& state, bave::Seconds dt) override;
	[[nodiscard]] auto is_destroyed() const -> bool override { return m_destroyed; }
	void draw(bave::Shader& shader) const override;

  protected:
	bave::NotNull<bave::IDisplay const*> m_display;
	Config m_config{};

	bave::RoundedQuadShape m_shape{};
	bool m_destroyed{};
};
} // namespace spaced
