#pragma once
#include <bave/core/inclusive_range.hpp>
#include <bave/core/time.hpp>
#include <bave/graphics/instanced.hpp>
#include <bave/graphics/sprite.hpp>
#include <bave/services/services.hpp>
#include <spaced/services/layout.hpp>

namespace spaced {
class StarField : public bave::IDrawable {
  public:
	struct Config {
		bave::Seconds spawn_rate{3s};
		bave::InclusiveRange<float> speed{10.0f, 50.0f};
		bave::InclusiveRange<float> scale{0.1f, 1.0f};
	};

	explicit StarField(bave::Services const& services);

	void add_field(std::shared_ptr<bave::Texture const> texture, Config const& config);

	void tick(bave::Seconds dt);
	void draw(bave::Shader& shader) const final;

	Config config{};

  private:
	struct Star {
		glm::vec2 position{};
		float scale{1.0f};
		float speed{};
		bave::Rgba tint{bave::white_v};
	};

	struct Field {
		Config config{};
		bave::Rect<> play_area{};
		bave::Instanced<bave::Sprite> sprite{};

		std::vector<Star> stars{};
		bave::Seconds next_spawn{};

		[[nodiscard]] auto spawn() const -> Star;

		void pre_warm();

		void tick(bave::Seconds dt);
		auto tick_stars(bave::Seconds dt) -> bool;
		void tick_spawn(bave::Seconds dt);
		void sync();
	};

	bave::NotNull<Layout const*> m_layout;

	std::vector<Field> m_fields{};
	bave::Seconds m_next_spawn{};
};
} // namespace spaced
