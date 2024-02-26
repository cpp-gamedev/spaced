#pragma once
#include <bave/graphics/drawable.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/services/services.hpp>
#include <spaced/ui/outline_quad.hpp>
#include <spaced/ui/widget.hpp>

namespace spaced::ui {
enum class ListBoxDir : int { eHorz, eVert };

template <std::derived_from<IElement> Type>
class ListBox : public IWidget {
  public:
	using Dir = ListBoxDir;

	explicit ListBox(Services const& services, Dir dir = Dir::eHorz) : m_layout(&services.get<ILayout>()), m_dir(dir) {}

	void push_back(Type item) {
		m_items.push_back(std::move(item));
		switch (m_dir) {
		case Dir::eHorz: rearrange_horz(); break;
		case Dir::eVert: rearrange_vert(); break;
		}
	}

	void clear() { m_items.clear(); }

	void shift_items(glm::vec2 delta) {
		if (m_items.empty()) { return; }
		switch (m_dir) {
		default:
		case Dir::eHorz: delta.y = 0.0f; break;
		case Dir::eVert: delta.x = 0.0f; break;
		}
		delta = glm::clamp(delta, get_min_delta(), get_max_delta());
		for (auto& item : m_items) { item.set_position(item.get_position() + delta); }
	}

	[[nodiscard]] auto get_items() const -> std::span<Type const> { return m_items; }
	[[nodiscard]] auto get_items() -> std::span<Type> { return m_items; }

	[[nodiscard]] auto get_size() const -> glm::vec2 override { return background.get_size(); }
	[[nodiscard]] auto get_position() const -> glm::vec2 override { return m_position; }

	void set_position(glm::vec2 const position) override {
		auto const delta = position - m_position;
		background.set_position(position);
		for (auto& item : m_items) { item.set_position(item.get_position() + delta); }
		m_position = position;
	}

	void on_move(bave::PointerMove const& pointer_move) override {
		if (m_drag_pos) {
			auto const delta = pointer_move.pointer.position - *m_drag_pos;
			shift_items(m_layout->project_to_world(delta));
			m_drag_pos = pointer_move.pointer.position;
			m_dragging = true;
			return;
		}
		if constexpr (std::derived_from<Type, IWidget>) {
			for (auto& item : m_items) { item.on_move(pointer_move); }
		}
	}

	void on_tap(bave::PointerTap const& pointer_tap) override {
		auto was_dragging = m_dragging;
		if (pointer_tap.pointer.id == bave::PointerId::ePrimary && pointer_tap.button == bave::MouseButton::eLeft) {
			switch (pointer_tap.action) {
			case bave::Action::ePress: {
				if (background.get_background().get_bounds().contains(m_layout->unproject(pointer_tap.pointer.position))) {
					m_drag_pos = pointer_tap.pointer.position;
				}
				break;
			}
			case bave::Action::eRelease: {
				m_drag_pos.reset();
				m_dragging = false;
				break;
			}
			default: break;
			}
		}
		if (was_dragging) { return; }
		if constexpr (std::derived_from<Type, IWidget>) {
			for (auto& item : m_items) { item.on_tap(pointer_tap); }
		}
	}

	void tick(bave::Seconds dt) override {
		if constexpr (std::derived_from<Type, IWidget>) {
			for (auto& item : m_items) { item.tick(dt); }
		}
	}

	void draw(bave::Shader& shader) const override {
		background.draw(shader);
		auto const& collect_bg = background.get_background();
		auto const collect_rect = bave::Rect<>::from_size(collect_bg.get_shape().size, collect_bg.transform.position);
		auto render_view = m_layout->get_main_view();
		render_view.n_scissor = render_view.to_n_scissor(collect_rect);
		shader.set_render_view(render_view);
		for (auto const& item : m_items) { item.draw(shader); }
		render_view.n_scissor = bave::uv_rect_v;
		shader.set_render_view(render_view);
	}

	OutlineQuad background{};
	float item_pad{100.0f};

  protected:
	[[nodiscard]] auto get_min_delta() const -> glm::vec2 {
		switch (m_dir) {
		default:
		case Dir::eHorz: return m_position - m_items.back().get_position();
		case Dir::eVert: return m_position - m_items.front().get_position();
		}
	}

	[[nodiscard]] auto get_max_delta() const -> glm::vec2 {
		switch (m_dir) {
		default:
		case Dir::eHorz: return m_position - m_items.front().get_position();
		case Dir::eVert: return m_position - m_items.back().get_position();
		}
	}

	void rearrange_horz() {
		if (m_items.size() == 1) {
			m_items.front().set_position(background.get_position());
			return;
		}

		auto const items_width = [&] {
			auto ret = float{};
			auto first = true;
			for (auto const& item : m_items) {
				ret += item.get_size().x;
				if (!first) { ret += item_pad; }
				first = false;
			}
			return ret;
		}();
		auto const bg_width = background.get_size().x;

		auto pos = background.get_position();
		pos.x += -0.5f * bg_width;
		for (auto& item : m_items) {
			auto const half_width = 0.5f * item.get_size().x;
			pos.x += half_width;
			item.set_position(pos);
			pos.x += half_width + item_pad;
		}
		if (items_width > bg_width) {
			shift_items({item_pad, 0.0f});
		} else {
			shift_items({0.5f * (bg_width - items_width), 0.0f});
		}
	}

	void rearrange_vert() {
		if (m_items.size() == 1) {
			m_items.front().set_position(background.get_position());
			return;
		}

		auto const items_height = [&] {
			auto ret = float{};
			auto first = true;
			for (auto const& item : m_items) {
				ret += item.get_size().y;
				if (!first) { ret += item_pad; }
				first = false;
			}
			return ret;
		}();
		auto const bg_height = background.get_size().y;

		auto pos = background.get_position();
		pos.y += 0.5f * bg_height;
		for (auto& item : m_items) {
			auto const half_height = 0.5f * item.get_size().y;
			pos.y -= half_height;
			item.set_position(pos);
			pos.y -= half_height + item_pad;
		}
		if (items_height > bg_height) {
			shift_items({0.0f, -item_pad});
		} else {
			shift_items({0.0f, -0.5f * (bg_height - items_height)});
		}
	}

	bave::NotNull<ILayout const*> m_layout;
	Dir m_dir;

	std::vector<Type> m_items{};
	glm::vec2 m_position{};
	std::optional<glm::vec2> m_drag_pos{};
	bool m_dragging{};
};
} // namespace spaced::ui
