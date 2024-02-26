#pragma once
#include <bave/core/time.hpp>
#include <bave/graphics/shader.hpp>
#include <bave/input/event.hpp>
#include <spaced/ui/element.hpp>

namespace spaced::ui {
class IWidget : public IElement {
  public:
	virtual void on_move(bave::PointerMove const& /*pointer_move*/) {}
	virtual void on_tap(bave::PointerTap const& /*pointer_tap*/) {}
	virtual void tick(bave::Seconds /*dt*/) {}
};
} // namespace spaced::ui
