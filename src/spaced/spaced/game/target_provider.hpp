#pragma once
#include <bave/core/not_null.hpp>
#include <spaced/game/damageable.hpp>
#include <span>

namespace spaced {
class ITargetProvider : public bave::Polymorphic {
  public:
	[[nodiscard]] virtual auto get_targets() const -> std::span<bave::NotNull<IDamageable*> const> = 0;
};
} // namespace spaced
