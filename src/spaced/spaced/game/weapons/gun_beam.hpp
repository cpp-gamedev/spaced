#pragma once
#include <spaced/game/weapon.hpp>

namespace spaced {
class GunBeam final : public Weapon {
  public:
	struct Config {
		float beam_height{5.0f};
		bave::Seconds fire_duration{2s};
		bave::Seconds reload_delay{1s};
		std::shared_ptr<bave::Texture const> beam_texture{};
		float dps{10.0f};
	};

	explicit GunBeam(bave::Services const& services);

	[[nodiscard]] auto is_idle() const -> bool final { return m_fire_remain <= 0s && m_reload_remain <= 0s; }
	[[nodiscard]] auto get_icon() const -> std::shared_ptr<bave::Texture const> final { return config.beam_texture; }

	void tick(bave::Seconds dt) final;

	Config config{};

  private:
	auto do_fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round> final;
	void do_inspect() final;

	bave::Seconds m_fire_remain{};
	bave::Seconds m_reload_remain{};
};
} // namespace spaced
