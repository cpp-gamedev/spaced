#pragma once
#include <bave/logger.hpp>
#include <bave/platform.hpp>
#include <bave/services/audio.hpp>
#include <bave/services/display.hpp>
#include <bave/services/services.hpp>
#include <spaced/game/weapon_round.hpp>

namespace spaced {
class Weapon : public bave::Polymorphic {
  public:
	using Round = IWeaponRound;

	explicit Weapon(bave::Services const& services, std::string name);

	[[nodiscard]] auto get_rounds_remaining() const -> int { return rounds; }
	[[nodiscard]] virtual auto get_icon() const -> std::shared_ptr<bave::Texture const> { return {}; }

	auto fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round>;
	[[nodiscard]] virtual auto is_idle() const -> bool = 0;

	virtual void tick(bave::Seconds dt) = 0;

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	int rounds{-1};

  protected:
	[[nodiscard]] auto get_display() const -> bave::IDisplay const& { return *m_display; }
	[[nodiscard]] auto get_audio() const -> bave::IAudio& { return *m_audio; }

	virtual auto do_fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round> = 0;
	virtual void do_inspect();

	bave::Logger m_log{};

  private:
	bave::NotNull<bave::IDisplay const*> m_display;
	bave::NotNull<bave::IAudio*> m_audio;
};
} // namespace spaced
