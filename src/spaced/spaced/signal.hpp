#pragma once
#include <functional>
#include <memory>
#include <vector>

namespace spaced {
template <typename T>
class Signal;

using SignalHandle = std::shared_ptr<void>;

template <typename... Args>
class Signal<void(Args...)> {
  public:
	using Callback = std::function<void(Args const&...)>;

	[[nodiscard]] auto connect(Callback callback) -> SignalHandle {
		auto handle = std::make_shared<int>();
		m_listeners.push_back(Listener{.callback = std::move(callback), .handle = handle});
		return handle;
	}

	void dispatch(Args const&... args) const {
		std::erase_if(m_listeners, [](Listener const& l) { return l.handle.expired(); });
		auto copy = m_listeners;
		for (auto const& listener : copy) { listener.callback(args...); }
	}

  private:
	struct Listener {
		Callback callback{};
		std::weak_ptr<void> handle{};
	};

	mutable std::vector<Listener> m_listeners{};
};
} // namespace spaced
