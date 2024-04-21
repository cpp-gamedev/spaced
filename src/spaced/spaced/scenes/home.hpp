#pragma once
#include <spaced/scene.hpp>

namespace spaced {
class Home : public Scene {
  public:
	explicit Home(bave::App& app, Services const& services);

  private:
	void start_loads() final;
	void on_loaded() final;

	void create_ui();
};
} // namespace spaced
