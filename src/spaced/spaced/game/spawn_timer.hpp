#pragma once
#include <bave/core/polymorphic.hpp>
#include <bave/core/time.hpp>

namespace spaced {
template <typename Type>
struct SpawnTimer : bave::Polymorphic {
	bave::Seconds spawn_rate{1s};

	bave::Seconds elapsed{};

	explicit SpawnTimer(bave::Seconds spawn_rate) : spawn_rate(spawn_rate) {}

	auto tick(bave::Seconds const dt) -> std::unique_ptr<Type> {
		auto ret = std::unique_ptr<Type>{};
		elapsed += dt;
		if (elapsed >= spawn_rate) {
			ret = spawn();
			elapsed = {};
		}
		return ret;
	}

	[[nodiscard]] virtual auto spawn() const -> std::unique_ptr<Type> = 0;
};
} // namespace spaced
