#include <bave/persistor.hpp>
#include <spaced/game/game_save.hpp>
#include <array>
#include <cstddef>
#include <cstring>
#include <span>

namespace spaced {
using bave::App;
using bave::NotNull;
using bave::Persistor;

namespace {
constexpr std::string_view uri_v{"userdata/game_save.bin"};
} // namespace

GameSave::GameSave(NotNull<App const*> app) : m_app(app) {
	auto const persistor = Persistor{*m_app};
	if (persistor.exists(uri_v)) {
		auto const blob_bytes = persistor.read_bytes(uri_v);
		std::memcpy(&m_blob, blob_bytes.data(), blob_bytes.size());
	}
}

GameSave::~GameSave() { save(); }

auto GameSave::save() -> bool {
	auto const persistor = Persistor{*m_app};
	auto blob_bytes = std::array<std::byte, sizeof(Blob)>{};
	std::memcpy(blob_bytes.data(), &m_blob, sizeof(Blob));
	return persistor.write_bytes(uri_v, blob_bytes);
}
} // namespace spaced
