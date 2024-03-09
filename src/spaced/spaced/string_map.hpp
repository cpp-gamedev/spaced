#pragma once
#include <bave/core/string_hash.hpp>
#include <unordered_map>

namespace spaced {
template <typename ValueType>
class StringMap : public std::unordered_map<std::string, ValueType, bave::StringHash, std::equal_to<>> {
  public:
	[[nodiscard]] auto get_or(std::string_view const key, ValueType const& fallback) const -> ValueType {
		if (auto const it = this->find(key); it != this->end()) { return it->second; }
		return fallback;
	}

	[[nodiscard]] auto get(std::string_view const key) const -> ValueType
		requires(std::is_default_constructible_v<ValueType>)
	{
		return get_or(key, ValueType{});
	}

	[[nodiscard]] auto operator[](std::string_view const key) const -> ValueType
		requires(std::is_default_constructible_v<ValueType>)
	{
		return get(key);
	}
};
} // namespace spaced
