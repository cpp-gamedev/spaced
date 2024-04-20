#include <bave/graphics/sprite.hpp>
#include <bave/imgui/im_text.hpp>
#include <spaced/game/weapons/gun_beam.hpp>
#include <spaced/services/styles.hpp>

namespace spaced {
using bave::im_text;
using bave::NotNull;
using bave::Ptr;
using bave::Rect;
using bave::Rgba;
using bave::Seconds;
using bave::Shader;
using bave::Sprite;

namespace {
class LaserCharge : public IWeaponRound {
  public:
	using Config = GunBeam::Config;

	explicit LaserCharge(NotNull<ILayout const*> layout, Config config, glm::vec2 const muzzle_position)
		: m_layout(layout), m_config(config), m_fire_remain(config.fire_duration) {
		m_ray.transform.position.y = muzzle_position.y;
	}

	[[nodiscard]] auto is_destroyed() const -> bool final { return m_fire_remain <= 0s; }

	void tick(State const& state, bave::Seconds dt) final {
		if (m_fire_remain > 0s) { m_fire_remain -= dt; }

		sort_entries(state.targets, state.muzzle_position);

		auto const world_space = m_layout->get_world_space();
		auto const left_x = state.muzzle_position.x;
		auto right_x = 0.5f * world_space.x;
		for (auto const& entry : m_entries) {
			if (entry.target->take_damage(m_config.dps * dt.count())) {
				right_x = entry.target->get_bounds().top_left().x;
				break;
			}
		}

		auto const left_y = 0.5f * m_config.beam_height;
		auto const right_y = -left_y;
		auto const rect = Rect<>{.lt = {left_x, left_y}, .rb = {right_x, right_y}};
		m_ray.transform.position.x = rect.centre().x;
		m_ray.transform.position.y = state.muzzle_position.y;
		m_ray.set_size(rect.size());
		m_ray.tint = m_config.beam_tint;

		update_scale();
	}

	void draw(Shader& shader) const final { m_ray.draw(shader); }

	void update_scale() {
		static constexpr auto delta_v{0.25f};

		auto const fire_elapsed = m_config.fire_duration - m_fire_remain;
		if (auto const expand_until = delta_v * m_config.fire_duration; fire_elapsed < expand_until) {
			m_ray.transform.scale.y = std::lerp(0.0f, 1.0f, fire_elapsed / expand_until);
			return;
		}

		if (auto const shrink_from = delta_v * m_config.fire_duration; m_fire_remain < shrink_from) {
			m_ray.transform.scale.y = std::lerp(0.0f, 1.0f, m_fire_remain / shrink_from);
			return;
		}

		m_ray.transform.scale.y = 1.0f;
	}

	void sort_entries(std::span<NotNull<IDamageable*> const> targets, glm::vec2 const muzzle) {
		m_entries.clear();
		m_entries.reserve(targets.size());
		for (auto target : targets) {
			auto const rect = target->get_bounds();
			if (muzzle.y > rect.top_left().y || muzzle.y < rect.bottom_right().y || muzzle.x > rect.top_left().x) { continue; }
			auto const distance = rect.top_left().x;
			m_entries.push_back(Entry{.target = target, .distance = distance});
		}
		std::sort(m_entries.begin(), m_entries.end(), [](Entry const& a, Entry const& b) { return a.distance < b.distance; });
	}

  private:
	struct Entry {
		bave::NotNull<IDamageable*> target;
		float distance{};
	};

	NotNull<ILayout const*> m_layout;
	Config m_config;

	Sprite m_ray{};
	Seconds m_fire_remain{};
	std::vector<Entry> m_entries{};
};
} // namespace

GunBeam::GunBeam(Services const& services) : Weapon(services, "GunBeam") {
	auto const& rgbas = services.get<Styles>().rgbas;
	config.beam_tint = rgbas.get_or("gun_beam", rgbas["grey"]);
}

void GunBeam::tick(Seconds const dt) {
	if (m_fire_remain > 0s) {
		m_fire_remain -= dt;
		if (m_fire_remain <= 0s) {
			m_fire_remain = 0s;
			m_reload_remain = config.reload_delay;
		}
	}
	if (m_reload_remain > 0s) { m_reload_remain -= dt; }
}

auto GunBeam::do_fire(glm::vec2 const muzzle_position) -> std::unique_ptr<Round> {
	if (!is_idle() || m_reload_remain > 0s || rounds == 0) { return {}; }

	if (rounds > 0) { --rounds; }
	m_fire_remain = config.fire_duration;
	m_reload_remain = 0s;
	return std::make_unique<LaserCharge>(&get_layout(), config, muzzle_position);
}

void GunBeam::do_inspect() {
	if constexpr (bave::imgui_v) {
		im_text("type: GunBeam");
		Weapon::do_inspect();
		ImGui::DragFloat("beam height", &config.beam_height, 0.25f, 1.0f, 100.0f);
		auto fire_duration = config.fire_duration.count();
		if (ImGui::DragFloat("fire duration (s)", &fire_duration, 0.25f, 0.25f, 10.0f)) { config.fire_duration = Seconds{fire_duration}; }
		auto reload_delay = config.reload_delay.count();
		if (ImGui::DragFloat("reload delay (s)", &reload_delay, 0.25f, 0.25f, 10.0f)) { config.reload_delay = Seconds{reload_delay}; }
		auto beam_tint = config.beam_tint.to_vec4();
		if (ImGui::ColorEdit4("beam tint", &beam_tint.x)) { config.beam_tint = Rgba::from(beam_tint); }
		ImGui::DragFloat("dps", &config.dps, 0.25f, 0.25f, 10.0f);
	}
}
} // namespace spaced
