#pragma once
#include <bave/services/audio.hpp>
#include <spaced/game/weapon.hpp>
#include <spaced/game/weapons/projectile.hpp>

namespace spaced {
class GunKinetic final : public Weapon {
  public:
	explicit GunKinetic(bave::Services const& services);

	[[nodiscard]] auto is_idle() const -> bool final { return m_reload_remain <= 0s; }
	[[nodiscard]] auto get_icon() const -> std::shared_ptr<bave::Texture const> final { return projectile_config.texture; }

	void tick(bave::Seconds dt) final;

	Projectile::Config projectile_config{};
	std::string_view fire_sfx{"assets/sfx/kinetic_fire.wav"};
	bave::Seconds reload_delay{0.15s};

  private:
	auto do_fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round> final;
	void do_inspect() final;

	bave::Seconds m_reload_remain{};
	bave::NotNull<bave::IAudio*> m_audio;
};
} // namespace spaced
