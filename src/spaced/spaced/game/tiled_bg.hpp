#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/instanced.hpp>
#include <bave/graphics/shape.hpp>
#include <bave/services/services.hpp>
#include <spaced/services/layout.hpp>

namespace spaced {
class TiledBg {
  public:
	explicit TiledBg(bave::Services const& services);

	void set_tile_size(glm::vec2 size);
	void set_texture(std::shared_ptr<bave::Texture const> texture) { m_quad.set_texture(std::move(texture)); }

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const;

	float x_speed{10.0f};

  private:
	bave::NotNull<Layout const*> m_layout;
	bave::Instanced<bave::QuadShape> m_quad{};

	float m_exit_x{};
};
} // namespace spaced
