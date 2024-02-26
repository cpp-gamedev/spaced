#pragma once
#include <spaced/scene.hpp>

namespace spaced {
class Home : public Scene {
  public:
	explicit Home(bave::App& app, Services const& services);

  private:
	void on_loaded() final;

	void create_ui();
};
} // namespace spaced
