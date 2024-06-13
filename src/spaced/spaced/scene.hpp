#pragma once
#include <bave/app.hpp>
#include <bave/core/polymorphic.hpp>
#include <bave/input/event_sink.hpp>
#include <bave/loader.hpp>
#include <spaced/async_exec.hpp>
#include <spaced/services/services.hpp>
#include <spaced/ui/loading_screen.hpp>
#include <spaced/ui/view.hpp>

namespace spaced {
class ISceneSwitcher;

class Scene : public bave::EventSink {
  public:
	void start_loading();

	void on_focus_event(bave::FocusChange const& focus_change);
	void on_key_event(bave::KeyInput const& key_input);
	void on_move_event(bave::PointerMove const& pointer_move);
	void on_tap_event(bave::PointerTap const& pointer_tap);
	void on_scroll_event(bave::MouseScroll const& mouse_scroll);
	void tick_frame(bave::Seconds dt);
	void render_frame() const;

	[[nodiscard]] virtual auto get_music_uri() const -> std::string_view { return {}; }

	[[nodiscard]] auto get_app() const -> bave::App& { return m_app; }
	[[nodiscard]] auto get_services() const -> Services const& { return m_services; }
	[[nodiscard]] auto make_loader() const -> bave::Loader { return bave::Loader{&m_app.get_data_store(), &m_app.get_render_device()}; }

	[[nodiscard]] auto is_loading() const -> bool { return m_loading_screen.has_value(); }
	[[nodiscard]] auto is_ui_blocking_input() const -> bool;

	void push_view(std::unique_ptr<ui::View> view);
	auto pop_view() -> std::unique_ptr<ui::View>;

	[[nodiscard]] auto get_switcher() const -> ISceneSwitcher&;

	bave::Rgba clear_colour{bave::black_v};

  protected:
	explicit Scene(bave::App& app, Services const& services, std::string name);

	virtual auto build_load_stages() -> std::vector<AsyncExec::Stage> { return {}; }
	virtual void on_loaded() {}

	virtual void tick(bave::Seconds /*dt*/) {}
	virtual void render(bave::Shader& /*shader*/) const {}

	bave::Logger m_log{};

  private:
	template <typename F>
	auto on_ui_event(F per_view) -> bool;

	auto cache_views() -> std::span<bave::Ptr<ui::View> const>;

	void update_loading(bave::Seconds dt);
	auto render_loading(bave::Shader& shader) const -> bool;

	bave::App& m_app;
	Services const& m_services;
	std::vector<std::unique_ptr<ui::View>> m_views{};
	std::vector<bave::Ptr<ui::View>> m_cached_views{};

	std::optional<AsyncExec> m_load{};
	std::optional<ui::LoadingScreen> m_loading_screen{};
};

class EmptyScene : public Scene {
  public:
	explicit EmptyScene(bave::App& app, Services const& services) : Scene(app, services, "EmptyScene") {}
};
} // namespace spaced
