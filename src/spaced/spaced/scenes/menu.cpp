#include <bave/services/display.hpp>
#include <bave/services/resources.hpp>
#include <bave/services/scene_switcher.hpp>
#include <bave/services/styles.hpp>
#include <bave/ui/button.hpp>
#include <bave/ui/dialog.hpp>
#include <bave/ui/text.hpp>
#include <spaced/build_version.hpp>
#include <spaced/game_title.hpp>
#include <spaced/prefs.hpp>
#include <spaced/scenes/endless.hpp>
#include <spaced/scenes/menu.hpp>
#include <spaced/services/layout.hpp>
#include <spaced/util.hpp>

namespace spaced {
using bave::App;
using bave::AssetManifest;
using bave::Display;
using bave::Seconds;
using bave::Services;
using bave::Text;
using bave::TextHeight;

namespace ui = bave::ui;

namespace {
using bave::Display;
using bave::PointerMove;
using bave::PointerTap;
using bave::QuadShape;
using bave::Rect;
using bave::Rgba;
using bave::Shader;
using bave::Styles;

struct FadeIn : ui::IWidget {
	int skip_frames{3};
	Seconds ttl{1s};

	QuadShape quad{};
	int frame{};
	Seconds elapsed{};

	explicit FadeIn(Services const& services) {
		auto shape = quad.get_shape();
		shape.size = services.get<Display>().world.get_size();
		quad.set_shape(shape);
		quad.tint = bave::black_v;
	}

	[[nodiscard]] auto get_size() const -> glm::vec2 final { return quad.get_shape().size; }
	[[nodiscard]] auto get_position() const -> glm::vec2 final { return quad.transform.position; }
	void set_position(glm::vec2 const position) final { quad.transform.position = position; }

	void on_move(PointerMove const& /*pointer_move*/) final {}
	void on_tap(PointerTap const& /*pointer_tap*/) final {}

	void tick(Seconds dt) final {
		++frame;
		if (frame < skip_frames) { return; }

		if (elapsed > ttl) {
			set_alpha(0.0f);
			return;
		}

		set_alpha((ttl - elapsed) / ttl);

		elapsed += dt;
	}

	void set_alpha(float const alpha) { quad.tint.channels.w = Rgba::to_u8(alpha); }

	void draw(Shader& shader) const final {
		if (elapsed > ttl) { return; }
		quad.draw(shader);
	}
};
} // namespace

MenuScene::MenuScene(App& app, Services const& services) : Scene(app, services, "Home") { clear_colour = services.get<Styles>().rgbas["bg_top"]; }

auto MenuScene::get_asset_manifest() -> AssetManifest {
	return AssetManifest{
		.audio_clips = {"music/menu.mp3"},
	};
}

void MenuScene::on_loaded() {
	switch_track("music/menu.mp3");
	create_ui();
}

void MenuScene::create_ui() {
	auto header = std::make_unique<ui::Text>(get_services());
	header->text.set_height(TextHeight{100}).set_string(std::string{game_title_v});
	header->set_position({0.0f, 300.0f});
	header->text.tint = bave::white_v;

	auto verstr = std::make_unique<ui::Text>(get_services());
	verstr->text.set_height(TextHeight{30}).set_string(to_string(build_version_v));
	auto const view_rect = Rect<>::from_size(get_services().get<Display>().world.get_size());
	auto const verpos = view_rect.bottom_right() + glm::vec2{-50.0f, 50.0f};
	verstr->set_position(verpos);
	verstr->text.set_align(Text::Align::eRight);

	static constexpr auto button_dy = 150.0f;
	auto button_y = 30.0f;
	auto next_button_y = [&] {
		auto const ret = button_y;
		button_y -= button_dy;
		return ret;
	};

	auto start = std::make_unique<ui::Button>(get_services());
	start->set_text("start");
	start->set_position({0.0f, next_button_y()});
	start->callback = [this]() { get_switcher().switch_to<EndlessScene>(); };

	auto options = std::make_unique<ui::Button>(get_services());
	options->set_text("options");
	options->set_position({0.0f, next_button_y()});
	options->callback = [this] { push_view(std::make_unique<Prefs::View>(&get_app(), get_services())); };

	auto quit = std::make_unique<ui::Button>(get_services());
	quit->set_text("quit");
	quit->set_position({0.0f, next_button_y()});
	quit->callback = [this]() { get_app().shutdown(); };

	auto view = std::make_unique<ui::View>();
	view->push(std::move(verstr));
	view->push(std::move(header));
	view->push(std::move(start));
	view->push(std::move(options));
	view->push(std::move(quit));
	view->render_view = get_services().get<Display>().world.render_view;

	view->push(std::make_unique<FadeIn>(get_services()));

	push_view(std::move(view));
}
} // namespace spaced
