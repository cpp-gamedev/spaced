#pragma once
#include <spaced/scene.hpp>

namespace spaced {
class Home : public Scene {
  public:
	static auto get_text_heights() -> std::vector<bave::TextHeight>;

	explicit Home(bave::App& app, Services const& services);

  private:
	void create_ui();
};
} // namespace spaced
