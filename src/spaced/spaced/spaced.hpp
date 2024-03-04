#pragma once
#include <bave/driver.hpp>
#include <spaced/scene.hpp>
#include <spaced/services/services.hpp>

namespace spaced {
class Serializer;

class Spaced : public bave::Driver {
  public:
	explicit Spaced(bave::App& app);

  private:
	static void set_bindings(Serializer& serializer);

	void on_focus(bave::FocusChange const& focus_change) final;
	void on_key(bave::KeyInput const& key_input) final;
	void on_move(bave::PointerMove const& pointer_move) final;
	void on_tap(bave::PointerTap const& pointer_tap) final;
	void on_scroll(bave::MouseScroll const& mouse_scroll) final;

	void tick() final;
	void render() const final;

	void load_resources();
	void create_services();
	void set_layout();
	void set_scene();

	bave::Logger m_log{"Hog"};
	Services m_services{};
	bave::Ptr<ILayout> m_layout{};
	std::unique_ptr<Scene> m_scene{};
};
} // namespace spaced
