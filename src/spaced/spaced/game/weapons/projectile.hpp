#pragma once
#include <bave/graphics/sprite_anim.hpp>
#include <spaced/game/weapon_round.hpp>
#include <spaced/services/layout.hpp>

namespace spaced {
class Projectile : public IWeaponRound {
  public:
	struct Config {
		std::shared_ptr<bave::Texture> texture{};
		std::shared_ptr<bave::TextureAtlas> atlas{};
		std::shared_ptr<bave::AnimTimeline> anim_timeline{};
		glm::vec2 size{50.0f, 5.0f};
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

	bave::SpriteAnim m_sprite{};
	bool m_destroyed{};
};
} // namespace spaced
