#include <djson/json.hpp>
#include <spaced/services/serializer.hpp>

namespace spaced {
auto Serializer::get_type_name(dj::Json const& json) const -> std::string_view {
	auto const ret = json["type_name"].as_string();
	if (ret.empty()) {
		m_log.error("JSON missing 'type_name'");
		return {};
	}
	return ret;
}

void Serializer::bind(Factory factory, std::string_view type_name) {
	if (!factory) {
		m_log.error("null factory");
		return;
	}
	if (type_name.empty()) {
		auto const temp = factory();
		if (!temp) {
			m_log.error("factory returned null serializable");
			return;
		}
		type_name = temp->get_type_name();
		if (type_name.empty()) {
			m_log.error("serializable returned empty type_name");
			return;
		}
	}
	m_factories.insert_or_assign(type_name, std::move(factory));
}

auto Serializer::deserialize(std::string_view const type_name, dj::Json const& json) const -> std::unique_ptr<ISerializable> {
	if (type_name.empty()) {
		m_log.error("JSON missing 'type_name'");
		return {};
	}

	auto const it = m_factories.find(type_name);
	if (it == m_factories.end()) {
		m_log.error("unrecognized type_name: '{}'", type_name);
		return {};
	}

	auto ret = it->second();
	if (!ret) {
		m_log.error("factory returned null for type_name: '{}'", type_name);
		return {};
	}

	ret->load_from(*this, json);
	return ret;
}

auto Serializer::serialize(ISerializable const& serializable) const -> dj::Json {
	auto const type_name = serializable.get_type_name();
	if (type_name.empty()) {
		m_log.error("cannot serialize empty type_name");
		return {};
	}

	auto ret = dj::Json{};
	ret["type_name"] = type_name;
	serializable.save_to(*this, ret);
	return ret;
}

auto Serializer::get_bound_type_names() const -> std::vector<std::string_view> {
	auto ret = std::vector<std::string_view>{};
	ret.reserve(m_factories.size());
	for (auto const& [name, _] : m_factories) { ret.push_back(name); }
	return ret;
}
} // namespace spaced
