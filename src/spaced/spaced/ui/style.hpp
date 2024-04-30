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
		float padding{30.0f};
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
	float corner_ratio{0.5f};
	float padding{10.0f};
};

struct DialogStyle {
	glm::vec2 footer_padding{20.0f, 10.0f};
	bave::Rgba background_tint{bave::yellow_v};
	bave::Rgba outline_tint{bave::cyan_v};
	bave::Rgba content_text_tint{bave::black_v};
};
} // namespace spaced::ui
