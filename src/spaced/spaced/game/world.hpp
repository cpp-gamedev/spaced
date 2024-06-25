#pragma once
#include <bave/graphics/particle_emitter.hpp>
#include <bave/services/audio.hpp>
#include <spaced/game/damageable.hpp>
#include <spaced/game/enemy.hpp>
#include <spaced/game/powerup.hpp>
#include <spaced/game/star_field.hpp>
#include <spaced/game/target_provider.hpp>

namespace bave {
struct Resources;
}

namespace spaced {
struct Stats;
struct SigPlayerScored;

class World : public ITargetProvider {
  public:
	struct CreateInfo {
		float starfield_density{1.0f};
	};

	explicit World(bave::NotNull<bave::Services const*> services, CreateInfo const& create_info);

	[[nodiscard]] auto get_targets() const -> std::span<bave::NotNull<IDamageable*> const> final { return m_targets; }

	[[nodiscard]] auto get_powerups() const -> std::span<bave::NotNull<Powerup*> const> { return m_powerups; }

	void tick(bave::Seconds dt, bool in_play);
	void draw(bave::Shader& shader) const;

	void push(std::unique_ptr<Enemy> enemy);
	void push(std::unique_ptr<Powerup> powerup);

	void inspect() {
		if constexpr (bave::debug_v) { do_inspect(); }
	}

  private:
	void on_death(Enemy const& enemy, bool add_score);

	void do_inspect();
	void inspect_enemies();

	bave::NotNull<bave::Services const*> m_services;
	bave::NotNull<bave::Resources const*> m_resources;
	bave::NotNull<bave::IAudio*> m_audio;
	bave::NotNull<Stats*> m_stats;
	bave::NotNull<SigPlayerScored*> m_on_player_scored;

	bave::CustomShape m_background{};
	StarField m_star_field;

	std::vector<std::unique_ptr<Enemy>> m_active_enemies{};
	std::vector<bave::ParticleEmitter> m_enemy_death_emitters{};
	std::vector<std::unique_ptr<Powerup>> m_active_powerups{};

	std::vector<bave::NotNull<IDamageable*>> m_targets{};
	std::vector<bave::NotNull<Powerup*>> m_powerups{};
};
} // namespace spaced
