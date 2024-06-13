#pragma once
#include <bave/driver.hpp>
#include <spaced/scene.hpp>
#include <spaced/services/scene_switcher.hpp>

namespace spaced {
class IAudio;

class GameDriver : public bave::Driver {
  public:
	explicit GameDriver(bave::App& app);

  protected:
	void on_focus(bave::FocusChange const& focus_change) override;
	void on_resize(bave::WindowResize const& window_resize) override;
	void on_resize(bave::FramebufferResize const& framebuffer_resize) override;
	void on_key(bave::KeyInput const& key_input) override;
	void on_char(bave::CharInput const& char_input) override;
	void on_tap(bave::PointerTap const& pointer_tap) override;
	void on_move(bave::PointerMove const& pointer_move) override;
	void on_scroll(bave::MouseScroll const& mouse_scroll) override;
	void on_drop(std::span<std::string const> paths) override;

	void tick() override;
	void render() const override;

	void switch_track(std::string_view from, std::string_view to) const;

	[[nodiscard]] auto get_switcher() const -> ISceneSwitcher& { return m_services.get<ISceneSwitcher>(); }

	Services m_services;

  private:
	struct SceneSwitcher;
	struct Layout;

	bave::Ptr<SceneSwitcher> m_switcher{};
	bave::Ptr<Layout> m_layout{};
	bave::Ptr<IAudio> m_audio{};

	std::unique_ptr<Scene> m_scene{};
};
} // namespace spaced
