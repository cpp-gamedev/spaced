#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/sprite.hpp>
#include <spaced/services/display.hpp>
#include <spaced/services/services.hpp>
#include <spaced/ui/progress_bar.hpp>

namespace spaced::ui {
class LoadingScreen {
  public:
	explicit LoadingScreen(Services const& services);

	void update(bave::Seconds dt, float progress);
	void draw(bave::Shader& shader) const;

  private:
	bave::NotNull<IDisplay const*> m_display;
	LoadingScreenStyle m_style{};
	bave::Sprite m_background{};
	bave::Sprite m_spinner{};
	ProgressBar m_progress_bar;
};
} // namespace spaced::ui
