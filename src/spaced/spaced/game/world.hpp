#pragma once
#include <djson/json.hpp>
#include <vector>

namespace spaced {
struct World {
	struct Player {
		std::string exhaust_emitter{};
	};

	std::string name{};
	std::string background_tint{};
	Player player{};
	std::vector<dj::Json> enemy_factories{};
};
} // namespace spaced
