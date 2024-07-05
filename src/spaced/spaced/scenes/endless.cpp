#include <bave/core/fixed_string.hpp>
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
using bave::FixedString;
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

auto roll_check(int const space) {
	if (space <= 0) { return true; }
	return random_in_range(0, space) == 0;
}

auto get_creep_spawn_rate(Seconds const elapsed) -> Seconds {
	static constexpr auto base_rate_v = 0.9s;
	if (elapsed < 50s) { return base_rate_v; }
	if (elapsed < 100s) { return 0.75f * base_rate_v; }
	return 0.6f * base_rate_v;
}

auto get_gunner_spawn_rate(Seconds const elapsed) -> Seconds {
	static constexpr auto base_rate_v = 9.3s;
	if (elapsed < 40s) { return base_rate_v; }
	if (elapsed < 80s) { return 0.75f * base_rate_v; }
	if (elapsed < 120s) { return 0.5f * base_rate_v; }
	return 0.25f * base_rate_v;
}

auto get_trooper_spawn_rate(Seconds const elapsed) -> Seconds {
	static constexpr auto base_rate_v = 29.4s;
	if (elapsed < 60s) { return base_rate_v; }
	if (elapsed < 100s) { return 0.5f * base_rate_v; }
	if (elapsed < 150s) { return 0.25f * base_rate_v; }
	if (elapsed < 180s) { return 0.125f * base_rate_v; }
	return 0.0625f * base_rate_v;
}
} // namespace

EndlessScene::EndlessScene(App& app, Services const& services) : GameScene(app, services), m_enemy_gun(services) {
	m_on_player_scored = services.get<GameSignals>().player_scored.connect([this](Enemy const& e) { on_player_scored(e); });
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

	on_start();
}

void EndlessScene::on_start() {
	m_elapsed = 0s;
	m_spawn_timers.clear();
	m_spawner.creep = m_spawn_timers.emplace_back(make_creep_spawner()).get();
	m_spawner.gunner = m_spawn_timers.emplace_back(make_gunner_spawner()).get();
	m_spawner.trooper = m_spawn_timers.emplace_back(make_trooper_spawner()).get();
}

void EndlessScene::tick(Seconds const dt) {
	GameScene::tick(dt);

	if (is_game_over()) { return; }

	m_elapsed += dt;
	m_since_powerup += dt;

	ramp_difficulty();

	for (auto const& spawn_timer : m_spawn_timers) {
		if (auto enemy = spawn_timer->tick(dt)) { push_enemy(std::move(enemy)); }
	}
}

void EndlessScene::ramp_difficulty() {
	m_spawner.creep->spawn_rate = get_creep_spawn_rate(m_elapsed);
	m_spawner.gunner->spawn_rate = get_gunner_spawn_rate(m_elapsed);
	m_spawner.trooper->spawn_rate = get_trooper_spawn_rate(m_elapsed);
}

void EndlessScene::on_player_scored(Enemy const& enemy) {
	if (m_since_powerup < 5s) { return; }

	if (!roll_check(30)) { return; }

	auto powerup = std::unique_ptr<Powerup>{};
	if (roll_check(20)) {
		powerup = make_1up_powerup();
	} else {
		powerup = make_beam_powerup();
	}

	if (!powerup) { return; }

	powerup->shape.transform.position = enemy.get_position();
	push_powerup(std::move(powerup));
	m_since_powerup = 0s;
}

auto EndlessScene::make_beam_powerup() const -> std::unique_ptr<Powerup> { return std::make_unique<powerup::Beam>(get_services()); }

auto EndlessScene::make_1up_powerup() const -> std::unique_ptr<Powerup> { return std::make_unique<powerup::OneUp>(get_services()); }

auto EndlessScene::make_creep_spawner() const -> std::unique_ptr<SpawnTimer<Enemy>> {
	return std::make_unique<CreepSpawner>(&get_services(), get_creep_spawn_rate(0s));
}

auto EndlessScene::make_gunner_spawner() -> std::unique_ptr<SpawnTimer<Enemy>> {
	return std::make_unique<GunnerSpawner>(&get_services(), &m_enemy_gun, get_gunner_spawn_rate(0s));
}

auto EndlessScene::make_trooper_spawner() -> std::unique_ptr<SpawnTimer<Enemy>> {
	return std::make_unique<TrooperSpawner>(&get_services(), &m_enemy_gun, get_trooper_spawn_rate(0s));
}

void EndlessScene::do_inspect(Seconds const /*dt*/) {
	if constexpr (bave::imgui_v) {
		auto elapsed = m_elapsed.count();
		if (ImGui::DragFloat("elapsed", &elapsed, 1.0f, 0.0f, 2000.0f)) { m_elapsed = Seconds{elapsed}; }
		if (ImGui::TreeNode("spawn timers")) {
			for (std::size_t i = 0; i < m_spawn_timers.size(); ++i) {
				if (ImGui::TreeNode(FixedString{"[{}]", i}.c_str())) {
					auto& spawn_timer = *m_spawn_timers.at(i);
					auto f = spawn_timer.spawn_rate.count();
					if (ImGui::DragFloat("period", &f, 0.25f, 0.25f, 20.0f)) { spawn_timer.spawn_rate = Seconds{f}; }
					f = spawn_timer.elapsed.count();
					if (ImGui::DragFloat("elapsed", &f, 0.25f, 0.25f, 20.0f)) { spawn_timer.elapsed = Seconds{f}; }
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
}
} // namespace spaced
