#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/sprite.hpp>
#include <bave/services/services.hpp>

namespace spaced {
class Shield : public bave::IDrawable {
  public:
	explicit Shield(bave::Services const& services);

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const final;

	void set_position(glm::vec2 const position) { m_sprite.transform.position = position; }

	[[nodiscard]] auto is_active() const -> bool { return ttl > 0s; }
	[[nodiscard]] auto get_bounds() const -> bave::Rect<> { return m_sprite.get_bounds(); }

	bave::Seconds ttl{};
	bave::Seconds fadeout{1s};

  private:
	bave::Sprite m_sprite{};
};
} // namespace spaced
