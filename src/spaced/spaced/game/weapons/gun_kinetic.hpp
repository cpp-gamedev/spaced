#pragma once
#include <spaced/game/weapon.hpp>
#include <spaced/game/weapons/projectile.hpp>

namespace spaced {
class GunKinetic final : public Weapon {
  public:
	explicit GunKinetic(Services const& services);

	auto fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round> final;
	[[nodiscard]] auto is_idle() const -> bool final { return m_reload_remain >= 0s; }

	void tick(bave::Seconds dt) final;

	Projectile::Config projectile_config{};
	bave::Seconds reload_delay{0.2s};

  private:
	void do_inspect() final;

	bave::Seconds m_reload_remain{};
};
} // namespace spaced
