#pragma once
#include <atomic>
#include <functional>
#include <future>
#include <span>
#include <vector>

namespace spaced {
class AsyncExec {
  public:
	struct Status;

	explicit AsyncExec(std::span<std::function<void()>> tasks);

	auto update() -> Status;

  private:
	std::vector<std::future<void>> m_remain{};
	std::atomic<int> m_completed{};
	int m_total{};
};

struct AsyncExec::Status {
	int remain{};
	int total{};

	[[nodiscard]] constexpr auto progress() const -> float {
		if (total == 0) { return 1.0f; }
		return static_cast<float>(total - remain) / static_cast<float>(total);
	}

	[[nodiscard]] constexpr auto is_complete() const -> bool { return remain == 0; }

	explicit constexpr operator bool() const { return is_complete(); }
};
} // namespace spaced
