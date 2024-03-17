#pragma once
#include <bave/app.hpp>
#include <bave/core/polymorphic.hpp>
#include <bave/loader.hpp>
#include <spaced/async_exec.hpp>
#include <spaced/services/services.hpp>
#include <spaced/ui/loading_screen.hpp>
#include <spaced/ui/view.hpp>

namespace spaced {
class Scene : public bave::PolyPinned {
  public:
	explicit Scene(bave::App& app, Services const& services, std::string name = "Scene");

	void on_focus_event(bave::FocusChange const& focus_change);
	void on_key_event(bave::KeyInput const& key_input);
	void on_move_event(bave::PointerMove const& pointer_move);
	void on_tap_event(bave::PointerTap const& pointer_tap);
	void on_scroll_event(bave::MouseScroll const& mouse_scroll);
	void tick_frame(bave::Seconds dt);
	void render_frame() const;

	[[nodiscard]] auto get_app() const -> bave::App& { return m_app; }
	[[nodiscard]] auto get_services() const -> Services const& { return m_services; }

	[[nodiscard]] auto is_loading() const -> bool { return m_loading_screen.has_value(); }
	[[nodiscard]] auto is_ui_blocking_input() const -> bool;

	[[nodiscard]] auto make_loader() const -> bave::Loader { return bave::Loader{&m_app.get_data_store(), &m_app.get_render_device()}; }

	void push_view(std::unique_ptr<ui::View> view);

	bave::Rgba clear_colour{bave::black_v};

  protected:
	virtual void on_focus(bave::FocusChange const& /*focus_change*/) {}
	virtual void on_resize(bave::WindowResize const& /*window_resize*/) {}
	virtual void on_resize(bave::FramebufferResize const& /*framebuffer_resize*/) {}
	virtual void on_key(bave::KeyInput const& /*key_input*/) {}
	virtual void on_char(bave::CharInput const& /*char_input*/) {}
	virtual void on_tap(bave::PointerTap const& /*pointer_tap*/) {}
	virtual void on_move(bave::PointerMove const& /*pointer_move*/) {}
	virtual void on_scroll(bave::MouseScroll const& /*mouse_scroll*/) {}
	virtual void on_drop(std::span<std::string const> /*paths*/) {}

	virtual void on_loaded() {}
	virtual void tick(bave::Seconds /*dt*/) {}
	virtual void render(bave::Shader& /*shader*/) const {}

	void add_load_tasks(std::span<AsyncExec::Task const> tasks);
	void add_load_stages(std::span<AsyncExec::Stage> task_stages);

	bave::Logger m_log{};

  private:
	template <typename F>
	auto on_ui_event(F per_view) -> bool;

	void update_loading(bave::Seconds dt);
	auto render_loading(bave::Shader& shader) const -> bool;

	bave::App& m_app;
	Services const& m_services;
	std::vector<std::unique_ptr<ui::View>> m_views{};

	std::optional<AsyncExec> m_load{};
	std::optional<ui::LoadingScreen> m_loading_screen{};
};
} // namespace spaced
