#pragma once
#include <bave/core/ptr.hpp>
#include <bave/logger.hpp>
#include <spaced/serializable.hpp>
#include <spaced/services/service.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

namespace spaced {
template <typename Type>
concept SerializableT = std::derived_from<Type, ISerializable>;

class Serializer : public IService {
  public:
	using Factory = std::function<std::unique_ptr<ISerializable>()>;

	template <SerializableT To, SerializableT From>
	[[nodiscard]] static auto try_cast(std::unique_ptr<From>& out) -> std::unique_ptr<To> {
		auto* ptr = dynamic_cast<To*>(out.get());
		if (ptr == nullptr) { return {}; }
		auto ret = std::unique_ptr<To>{ptr};
		out.release();
		return ret;
	}

	[[nodiscard]] auto get_type_name(dj::Json const& json) const -> std::string_view;

	void bind(Factory factory, std::string_view type_name = {});

	template <SerializableT Type>
		requires(std::is_default_constructible_v<Type>)
	void bind() {
		bind([] { return std::make_unique<Type>(); }, Type{}.get_type_name());
	}

	[[nodiscard]] auto deserialize(dj::Json const& json) const -> std::unique_ptr<ISerializable> { return deserialize(get_type_name(json), json); }

	template <SerializableT Type>
	[[nodiscard]] auto deserialize_as(dj::Json const& json) const -> std::unique_ptr<Type> {
		auto const type_name = get_type_name(json);
		auto base = deserialize(type_name, json);
		auto ret = try_cast<Type>(base);
		if (ret == nullptr) {
			m_log.error("type mismatch for '{}'", type_name);
			return {};
		}
		return ret;
	}

	[[nodiscard]] auto serialize(ISerializable const& serializable) const -> dj::Json;

	[[nodiscard]] auto get_binding_count() const -> std::size_t { return m_factories.size(); }
	[[nodiscard]] auto get_bound_type_names() const -> std::vector<std::string_view>;

  private:
	[[nodiscard]] auto deserialize(std::string_view type_name, dj::Json const& json) const -> std::unique_ptr<ISerializable>;

	bave::Logger m_log{"Serializer"};
	std::unordered_map<std::string_view, Factory> m_factories{};
};
} // namespace spaced
