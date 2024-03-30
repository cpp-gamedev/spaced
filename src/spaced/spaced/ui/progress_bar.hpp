#pragma once
#include <spaced/services/services.hpp>
#include <spaced/ui/outline_quad.hpp>
#include <spaced/ui/style.hpp>
#include <spaced/ui/widget.hpp>

namespace spaced::ui {
class ProgressBar : public IWidget {
  public:
	using Style = ProgressBarStyle;

	explicit ProgressBar(Services const& services);

	glm::vec2 size{200.0f, 20.0f};
	glm::vec2 position{};

	void draw(bave::Shader& shader) const final;

	[[nodiscard]] auto get_progress() const -> float { return m_progress; }
	void set_progress(float progress);

	[[nodiscard]] auto get_style() const -> Style const& { return m_style; }
	void set_style(Style style);

	[[nodiscard]] auto get_size() const -> glm::vec2 final { return size; }
	[[nodiscard]] auto get_position() const -> glm::vec2 final { return position; }
	void set_position(glm::vec2 position) final { this->position = position; }

  private:
	Style m_style{};

	bave::RoundedQuadShape m_background{};
	bave::QuadShape m_fill{};

	float m_progress{};
};
} // namespace spaced::ui
