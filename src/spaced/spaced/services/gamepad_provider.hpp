#pragma once
#include <bave/input/gamepad.hpp>
#include <bave/services/service.hpp>

namespace spaced {
class IGamepadProvider : public bave::IService {
  public:
	[[nodiscard]] virtual auto get_gamepad() const -> bave::Gamepad const& = 0;
};
} // namespace spaced
