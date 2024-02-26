#include <spaced/async_exec.hpp>

namespace spaced {
using namespace std::chrono_literals;

AsyncExec::AsyncExec(std::span<std::function<void()>> tasks) {
	if (tasks.empty()) { return; }

	m_remain.reserve(tasks.size());
	for (auto& task : tasks) {
		auto func = [task = std::move(task), this] {
			task();
			++m_completed;
		};
		m_remain.push_back(std::async(std::move(func)));
	}

	m_total = static_cast<int>(m_remain.size());
}

auto AsyncExec::update() -> Status {
	if (m_remain.empty()) { return Status{.remain = 0, .total = m_total}; }
	std::erase_if(m_remain, [](std::future<void> const& future) { return !future.valid() || future.wait_for(0s) == std::future_status::ready; });
	return Status{.remain = static_cast<int>(m_remain.size()), .total = m_total};
}
} // namespace spaced
