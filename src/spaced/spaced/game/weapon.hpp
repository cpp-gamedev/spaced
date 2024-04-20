#pragma once
#include <bave/logger.hpp>
#include <bave/platform.hpp>
#include <spaced/game/weapon_round.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>

namespace spaced {
class IAudio;

class Weapon : public bave::Polymorphic {
  public:
	using Round = IWeaponRound;

	explicit Weapon(Services const& services, std::string name);

	[[nodiscard]] auto get_rounds_remaining() const -> int { return rounds < 0 ? 1 : rounds; }

	auto fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round>;
	[[nodiscard]] virtual auto is_idle() const -> bool = 0;

	virtual void tick(bave::Seconds dt) = 0;

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

	int rounds{-1};

  protected:
	[[nodiscard]] auto get_layout() const -> ILayout const& { return *m_layout; }

	virtual auto do_fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round> = 0;
	virtual void do_inspect();

	bave::Logger m_log{};
	std::vector<std::string> m_fire_sfx{};

  private:
	bave::NotNull<ILayout const*> m_layout;
	bave::NotNull<IAudio*> m_audio;
};
} // namespace spaced
