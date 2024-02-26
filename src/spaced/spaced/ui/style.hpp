#pragma once
#include <bave/core/enum_array.hpp>
#include <bave/core/radians.hpp>
#include <bave/font/text_height.hpp>
#include <bave/graphics/rgba.hpp>
#include <glm/vec2.hpp>

namespace spaced::ui {
enum class ButtonState { eIdle, eHover, ePress, eCOUNT_ };

struct LoadingScreenStyle {
	bave::Rgba background_tint{bave::black_v};
	struct {
		glm::vec2 size{100.0f};
		bave::Degrees spin_rate{720.0f};
	} spinner{};
	struct {
		float n_width{0.9f};
		float height{50.0f};
		float bottom_offset{200.0f};
		float outline_width{10.0f};
	} progress_bar{};
};

struct ButtonStyle {
	using Tint = bave::EnumArray<ButtonState, bave::Rgba>;

	Tint background{bave::white_v, bave::yellow_v, bave::cyan_v};
	Tint outline{bave::black_v, bave::black_v, bave::black_v};
	Tint text{bave::black_v, bave::black_v, bave::black_v};
	float outline_width{10.0f};
	float corner_ratio{0.33f};
};

struct ProgressBarStyle {
	bave::Rgba background{bave::white_v};
	bave::Rgba fill{bave::yellow_v};
	bave::Rgba outline{bave::red_v};
	float outline_width{5.0f};
};

struct CollectListItemStyle {
	float panel_diameter{60.0f};
	bave::Rgba panel_rgba{bave::white_v};
	float outline_width{5.0f};
	bave::Rgba outline_rgba{bave::red_v};
	glm::vec2 sprite_size{40.0f};
	float panel_y_offset{12.0f};
	bave::TextHeight text_height = bave::TextHeight{20};
	bave::Rgba text_rgba{bave::black_v};
	float text_y_offset{-43.0f};
	float item_x_pad{100.0f};
};
} // namespace spaced::ui
