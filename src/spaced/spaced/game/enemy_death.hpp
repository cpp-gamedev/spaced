#pragma once
#include <bave/core/polymorphic.hpp>
#include <glm/vec2.hpp>
#include <cstdint>

namespace spaced {
struct EnemyDeath {
	glm::vec2 position{};
	std::int64_t points{};
};

class IEnemyDeathListener : public bave::Polymorphic {
  public:
	virtual void on_death(EnemyDeath const& death) = 0;
};
} // namespace spaced
