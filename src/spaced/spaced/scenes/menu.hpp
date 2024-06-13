#pragma once
#include <spaced/scene.hpp>

namespace spaced {
class MenuScene : public Scene {
  public:
	static auto get_text_heights() -> std::vector<bave::TextHeight>;

	explicit MenuScene(bave::App& app, Services const& services);

  private:
	void create_ui();

	[[nodiscard]] auto get_music_uri() const -> std::string_view final { return "music/menu.mp3"; }
};
} // namespace spaced