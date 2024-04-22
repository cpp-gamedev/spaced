#pragma once
#include <spaced/async_exec.hpp>
#include <spaced/scene.hpp>
#include <spaced/ui/loading_screen.hpp>

namespace spaced {
class LoadAssets : public Scene {
  public:
	explicit LoadAssets(bave::App& app, Services const& services);

  private:
	void on_loaded();

	void tick(bave::Seconds dt) final;
	void render(bave::Shader& shader) const final;

	ui::LoadingScreen m_loading_screen;
	AsyncExec m_load;
};
} // namespace spaced
