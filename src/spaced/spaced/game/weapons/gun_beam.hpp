#pragma once
#include <spaced/game/weapon.hpp>

namespace spaced {
class GunBeam final : public Weapon {
  public:
	struct Config {
		float beam_height{5.0f};
		bave::Seconds fire_duration{2s};
		bave::Seconds reload_delay{1s};
		bave::Rgba beam_tint{bave::red_v};
		float dps{10.0f};
	};

	explicit GunBeam(bave::Services const& services);

	[[nodiscard]] auto is_idle() const -> bool final { return m_fire_remain <= 0s; }
	[[nodiscard]] auto get_icon() const -> std::shared_ptr<bave::Texture const> final { return m_icon; }

	void tick(bave::Seconds dt) final;

	Config config{};

  private:
	auto do_fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round> final;
	void do_inspect() final;

	std::shared_ptr<bave::Texture const> m_icon{};

	bave::Seconds m_fire_remain{};
	bave::Seconds m_reload_remain{};
};
} // namespace spaced
