#pragma once
#include <bave/core/not_null.hpp>
#include <bave/core/ptr.hpp>
#include <spaced/services/service.hpp>
#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>

namespace spaced {
/// \brief Concept constraining Type to a subclass of IService.
template <typename Type>
concept ServiceT = std::derived_from<Type, IService>;

/// \brief Container of stored services.
///
/// Intended usage:
/// 1. Own an instance in main / app Driver.
/// 2. Bind services to subclasses. (From and To being the same type is also OK.)
/// 3. Pass a const reference of Services to dependencies
/// Point 3 ensures dependencies cannot bind new / remove existing services,
/// but can locate and use already bound ones.
class Services {
  public:
	/// \brief Bind a service instance to a type (From).
	/// \param service Concrete instance of service to bind.
	/// \pre service must not be null, From must not already be bound.
	template <ServiceT From, std::derived_from<From> To>
	void bind(std::unique_ptr<To> service) {
		if (!service) { throw std::runtime_error{"Attempt to bind null service"}; }
		static auto const index = std::type_index{typeid(From)};
		auto lock = std::scoped_lock{m_mutex};
		if (m_services.contains(index)) { throw std::runtime_error{"Attempt to bind duplicate service"}; }
		m_services.insert_or_assign(index, std::move(service));
	}

	/// \brief Remove a bound service instance. Type need not actually be bound.
	template <ServiceT Type>
	void remove() {
		static auto const index = std::type_index{typeid(Type)};
		auto lock = std::scoped_lock{m_mutex};
		m_services.erase(index);
	}

	/// \brief Remove all bound service instances.
	void remove_all() {
		auto lock = std::scoped_lock{m_mutex};
		m_services.clear();
	}

	/// \brief Check if a service is bound.
	/// \returns true if bound.
	template <ServiceT Type>
	[[nodiscard]] auto contains() const -> bool {
		return find<Type>() != nullptr;
	}

	/// \brief Locate a service instance if bound.
	/// \returns Pointer to service instance if bound, else nullptr.
	template <ServiceT Type>
	[[nodiscard]] auto find() const -> bave::Ptr<Type> {
		static auto const index = std::type_index{typeid(Type)};
		auto lock = std::scoped_lock{m_mutex};
		if (auto const it = m_services.find(index); it != m_services.end()) { return static_cast<Type*>(it->second.get()); }
		return {};
	}

	/// \brief Obtain a bound service.
	/// \returns Mutable reference to bound service.
	/// \pre Type must be bound.
	template <ServiceT Type>
	[[nodiscard]] auto get() const -> Type& {
		auto ret = find<Type>();
		if (!ret) { throw std::runtime_error{"Service not found"}; }
		return *ret;
	}

	/// \brief Obtain the count of bound services.
	/// \returns Count of bound services.
	[[nodiscard]] auto size() const -> std::size_t {
		auto lock = std::scoped_lock{m_mutex};
		return m_services.size();
	}

  private:
	std::unordered_map<std::type_index, std::unique_ptr<IService>> m_services{};
	mutable std::mutex m_mutex{};
};
} // namespace spaced
