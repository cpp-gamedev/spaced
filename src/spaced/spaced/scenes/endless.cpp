#include <bave/core/random.hpp>
#include <bave/services/resources.hpp>
#include <spaced/game/enemies/creep.hpp>
#include <spaced/game/enemies/gunner.hpp>
#include <spaced/game/enemies/trooper.hpp>
#include <spaced/game/powerups/beam.hpp>
#include <spaced/game/powerups/one_up.hpp>
#include <spaced/scenes/endless.hpp>
#include <spaced/services/game_signals.hpp>

namespace spaced {
using bave::App;
using bave::NotNull;
using bave::random_in_range;
using bave::Resources;
using bave::Seconds;
using bave::Services;
using bave::Texture;

namespace {
struct CreepSpawner : SpawnTimer<Enemy> {
	NotNull<Services const*> services;

	explicit CreepSpawner(NotNull<Services const*> services, Seconds spawn_rate) : SpawnTimer(spawn_rate), services(services) {}

	[[nodiscard]] auto spawn() const -> std::unique_ptr<Enemy> final { return std::make_unique<enemy::Creep>(*services); }
};

template <std::derived_from<enemy::GunnerBase> T>
struct GunnerSpawnerT : SpawnTimer<Enemy> {
	NotNull<Services const*> services;
	NotNull<GunKinetic*> gun;

	explicit GunnerSpawnerT(NotNull<Services const*> services, NotNull<GunKinetic*> gun, Seconds spawn_rate)
		: SpawnTimer(spawn_rate), services(services), gun(gun) {}

	[[nodiscard]] auto spawn() const -> std::unique_ptr<Enemy> final { return std::make_unique<T>(*services, gun); }
};

using GunnerSpawner = GunnerSpawnerT<enemy::Gunner>;
using TrooperSpawner = GunnerSpawnerT<enemy::Trooper>;
} // namespace

EndlessScene::EndlessScene(App& app, Services const& services) : GameScene(app, services), m_enemy_gun(services) {
	m_on_player_scored = services.get<GameSignals>().player_scored.connect([this](Enemy const& e) {
		if (random_in_range(0, 10) < 3) { debug_spawn_powerup(e.get_position()); }
	});
}

void EndlessScene::on_loaded() {
	GameScene::on_loaded();

	auto const& resources = get_services().get<Resources>();

	m_enemy_gun.reload_delay = 0s;
	m_enemy_gun.fire_sfx = "sfx/kinetic_fire1.wav";
	m_enemy_gun.projectile_config.texture = resources.get<Texture>("images/round_kinetic_red.png");
	m_enemy_gun.projectile_config.instigator = Instigator::eEnemy;
	m_enemy_gun.projectile_config.x_speed = -m_enemy_gun.projectile_config.x_speed;
	m_enemy_gun.projectile_config.x_scale = -1.0f;

	m_spawn_timers.push_back(std::make_unique<CreepSpawner>(&get_services(), 1s));
	m_spawn_timers.push_back(std::make_unique<GunnerSpawner>(&get_services(), &m_enemy_gun, 2s));
	m_spawn_timers.push_back(std::make_unique<TrooperSpawner>(&get_services(), &m_enemy_gun, 5s));
}

void EndlessScene::tick(Seconds const dt) {
	GameScene::tick(dt);

	if (is_game_over()) { return; }

	for (auto const& spawn_timer : m_spawn_timers) {
		if (auto enemy = spawn_timer->tick(dt)) { push_enemy(std::move(enemy)); }
	}
}

void EndlessScene::debug_spawn_powerup(glm::vec2 const position) {
	auto powerup = std::unique_ptr<Powerup>{};
	if (random_in_range(0, 1) == 0) {
		powerup = std::make_unique<powerup::Beam>(get_services());
	} else {
		powerup = std::make_unique<powerup::OneUp>(get_services());
	}
	powerup->shape.transform.position = position;
	push_powerup(std::move(powerup));
}
} // namespace spaced
