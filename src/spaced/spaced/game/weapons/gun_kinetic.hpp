#pragma once
#include <bave/services/audio.hpp>
#include <spaced/game/weapon.hpp>
#include <spaced/game/weapons/projectile.hpp>

namespace spaced {
class GunKinetic final : public Weapon {
  public:
	explicit GunKinetic(bave::Services const& services);

	[[nodiscard]] auto is_idle() const -> bool final { return m_reload_remain <= 0s; }

	void tick(bave::Seconds dt) final;

	Projectile::Config projectile_config{};
	bave::Seconds reload_delay{0.25s};

  private:
	auto do_fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round> final;
	void do_inspect() final;

	bave::Seconds m_reload_remain{};
	bave::NotNull<bave::IAudio*> m_audio;
};
} // namespace spaced
