#pragma once
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>
#include <spaced/ui/widget.hpp>
#include <memory>
#include <vector>

namespace spaced::ui {
class View : public bave::Polymorphic {
  public:
	explicit View(Services const& services);

	void push(std::unique_ptr<IElement> element);

	[[nodiscard]] auto is_destroyed() const -> bool { return m_destroyed; }
	void set_destroyed() { m_destroyed = true; }

	virtual void on_move(bave::PointerMove const& pointer_move);
	virtual void on_tap(bave::PointerTap const& pointer_tap);
	virtual void tick(bave::Seconds dt);
	virtual void render(bave::Shader& shader) const;

	bool block_input_events{true};

  protected:
	std::vector<std::unique_ptr<ui::IElement>> m_elements{};
	std::vector<bave::Ptr<ui::IWidget>> m_widgets{};
	bave::NotNull<ILayout const*> m_layout;
	bool m_destroyed{};
};
} // namespace spaced::ui
