#pragma once
#include <bave/core/not_null.hpp>
#include <cstdint>

namespace bave {
class App;
}

namespace spaced {
class GameSave {
  public:
	GameSave(GameSave const&) = delete;
	GameSave(GameSave&&) = delete;
	auto operator=(GameSave const&) = delete;
	auto operator=(GameSave&&) = delete;

	explicit GameSave(bave::NotNull<bave::App const*> app);
	~GameSave();

	[[nodiscard]] auto get_hi_score() const -> std::int64_t { return m_blob.hi_score; }
	void set_hi_score(std::int64_t const hi_score) { m_blob.hi_score = hi_score; }

	auto save() -> bool;

  private:
	struct Blob {
		// struct must remain backwards compatible:
		// fields must NEVER be removed / reordered!
		// new fields can be added at the bottom.

		std::int64_t hi_score{};
	};

	bave::NotNull<bave::App const*> m_app;
	Blob m_blob{};
};
} // namespace spaced
