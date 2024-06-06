#pragma once
#include <spaced/services/styles.hpp>
#include <spaced/ui/progress_bar.hpp>
#include <spaced/ui/style.hpp>
#include <spaced/ui/widget.hpp>
#include <functional>

namespace spaced::ui {
class Slider : public IWidget {
  public:
	using Style = SliderStyle;

	explicit Slider(Services const& services);

	[[nodiscard]] auto get_size() const -> glm::vec2 final;
	[[nodiscard]] auto get_position() const -> glm::vec2 final { return m_progress_bar.get_position(); }
	void set_position(glm::vec2 position) final;

	[[nodiscard]] auto get_value() const -> float { return m_value; }
	void set_value(float value);

	[[nodiscard]] auto get_style() const -> Style { return m_style; }
	void set_style(Style style);

	std::function<void(float)> on_change{};

  protected:
	void draw(bave::Shader& shader) const final;
	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;

	[[nodiscard]] auto hit_test(glm::vec2 position) const -> bool;
	void track(glm::vec2 pointer);

	void update_value(float value);

	bave::NotNull<Styles const*> m_styles;
	Style m_style{};

	ui::ProgressBar m_progress_bar;
	bave::CircleShape m_knob{};
	float m_value{};

	bool m_pointer_down{};
};
} // namespace spaced::ui
