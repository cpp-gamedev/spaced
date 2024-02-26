#pragma once
#include <bave/graphics/text.hpp>
#include <spaced/services/services.hpp>
#include <spaced/ui/element.hpp>

namespace spaced::ui {
class Text : public IElement {
  public:
	explicit Text(Services const& services);

	[[nodiscard]] auto get_size() const -> glm::vec2 override { return text.get_bounds().size(); }
	[[nodiscard]] auto get_position() const -> glm::vec2 override { return text.transform.position; }
	void set_position(glm::vec2 position) override { text.transform.position = position; }
	void draw(bave::Shader& shader) const override { text.draw(shader); }

	bave::Text text{};
};
} // namespace spaced::ui
