#pragma once
#include <spaced/services/service.hpp>
#include <cstdint>

namespace dj {
class Json;
}

namespace spaced {
struct Stats : IService {
	struct {
		std::int64_t play_count{};
	} game{};

	struct {
		std::int64_t powerups_collected{};
		std::int64_t shots_fired{};
		std::int64_t enemies_poofed{};
		std::int64_t death_count{};
	} player{};

	std::int64_t run_count{};

	void from(dj::Json const& json);
	void to(dj::Json& out) const;
};
} // namespace spaced
