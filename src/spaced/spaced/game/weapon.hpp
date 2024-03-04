#pragma once
#include <bave/logger.hpp>
#include <bave/platform.hpp>
#include <spaced/game/weapon_round.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>

namespace spaced {
class Weapon : public bave::Polymorphic {
  public:
	using Round = IWeaponRound;

	explicit Weapon(Services const& services, std::string name) : m_log{std::move(name)}, m_layout(&services.get<ILayout>()) {}

	virtual auto fire(glm::vec2 muzzle_position) -> std::unique_ptr<Round> = 0;
	[[nodiscard]] virtual auto is_idle() const -> bool = 0;

	virtual void tick(bave::Seconds dt) = 0;

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

  protected:
	[[nodiscard]] auto get_layout() const -> ILayout const& { return *m_layout; }
	virtual void do_inspect() {}

	bave::Logger m_log{};

  private:
	bave::NotNull<ILayout const*> m_layout;
};
} // namespace spaced
