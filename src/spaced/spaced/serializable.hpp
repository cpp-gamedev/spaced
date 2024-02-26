#pragma once
#include <bave/core/polymorphic.hpp>
#include <string_view>

namespace dj {
class Json;
}

namespace spaced {
class Serializer;

class ISerializable : public bave::Polymorphic {
  public:
	[[nodiscard]] virtual auto get_type_name() const -> std::string_view = 0;
	virtual void load_from(Serializer const& /*serializer*/, dj::Json const& /*json*/) {}
	virtual void save_to(Serializer const& /*serializer*/, dj::Json& /*out*/) const {}
};
} // namespace spaced
