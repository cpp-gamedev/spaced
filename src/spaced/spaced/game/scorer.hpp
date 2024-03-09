#pragma once
#include <bave/core/polymorphic.hpp>
#include <cstdint>

namespace spaced {
class IScorer : public bave::Polymorphic {
  public:
	[[nodiscard]] virtual auto get_score() const -> std::int64_t = 0;
	virtual void add_score(std::int64_t score) = 0;
};
} // namespace spaced
