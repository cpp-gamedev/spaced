#pragma once
#include <bave/input/gamepad.hpp>
#include <spaced/services/service.hpp>

namespace spaced {
class IGamepadProvider : public IService {
  public:
	[[nodiscard]] virtual auto get_gamepad() const -> bave::Gamepad const& = 0;
};
} // namespace spaced
