#include <spaced/async_exec.hpp>
#include <cassert>
#include <iterator>
#include <numeric>

namespace spaced {
using namespace std::chrono_literals;

AsyncExec::AsyncExec(std::span<Task const> tasks) {
	if (tasks.empty()) { return; }

	m_total = static_cast<int>(tasks.size());
	enqueue(tasks);
}

AsyncExec::AsyncExec(std::span<Stage> stages) {
	if (stages.empty()) { return; }
	std::move(stages.begin(), stages.end(), std::back_inserter(m_stages));
	m_total = std::accumulate(m_stages.begin(), m_stages.end(), 0, [](int count, auto const& tasks) { return static_cast<int>(tasks.size()) + count; });
	start_next_stage();
}

auto AsyncExec::update() -> Status {
	if (m_remain.empty()) {
		if (m_stages.empty()) { return Status{.remain = 0, .total = m_total}; }
		start_next_stage();
	}
	std::erase_if(m_remain, [](std::future<void> const& future) { return !future.valid() || future.wait_for(0s) == std::future_status::ready; });
	return Status{.remain = m_total - m_completed, .total = m_total};
}

void AsyncExec::start_next_stage() {
	if (m_stages.empty()) { return; }

	auto get_next_stage = [&] {
		auto ret = std::move(m_stages.front());
		m_stages.pop_front();
		return ret;
	};

	auto stage = get_next_stage();
	while (stage.empty() && !m_stages.empty()) { stage = get_next_stage(); }

	enqueue(stage);
}

void AsyncExec::enqueue(std::span<Task const> tasks) {
	assert(m_remain.empty());
	if (tasks.empty()) { return; }

	m_remain.reserve(tasks.size());
	for (auto const& task : tasks) {
		auto func = [task = task, this] {
			task();
			++m_completed;
		};
		m_remain.push_back(std::async(std::move(func)));
	}
}
} // namespace spaced
