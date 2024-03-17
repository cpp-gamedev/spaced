#pragma once
#include <atomic>
#include <deque>
#include <functional>
#include <future>
#include <span>
#include <vector>

namespace spaced {
class AsyncExec {
  public:
	using Task = std::function<void()>;
	using Stage = std::vector<Task>;

	struct Status;

	explicit AsyncExec(std::span<Task const> tasks);
	explicit AsyncExec(std::span<Stage> stages);

	auto update() -> Status;

  private:
	void start_next_stage();
	void enqueue(std::span<Task const> tasks);

	std::vector<std::future<void>> m_remain{};
	std::deque<Stage> m_stages{};
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
