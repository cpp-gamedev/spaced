#include <imgui.h>
#include <bave/imgui/im_text.hpp>
#include <bave/platform.hpp>
#include <spaced/game/player.hpp>
#include <spaced/services/resources.hpp>

// temp for testing
#include <spaced/game/weapons/gun_beam.hpp>
#include <spaced/game/weapons/gun_kinetic.hpp>

namespace spaced {
using bave::Degrees;
using bave::im_text;
using bave::NotNull;
using bave::ParticleEmitter;
using bave::PointerMove;
using bave::PointerTap;
using bave::RoundedQuad;
using bave::Seconds;
using bave::Shader;

Player::Player(Services const& services, std::unique_ptr<IController> controller) : m_services(&services), m_controller(std::move(controller)) {
	setup_ship();
	setup_foam();

	debug_switch_weapon();
}

void Player::on_focus(bave::FocusChange const& /*focus_changed*/) { m_controller->untap(); }

void Player::on_move(PointerMove const& pointer_move) { m_controller->on_move(pointer_move); }

void Player::on_tap(PointerTap const& pointer_tap) { m_controller->on_tap(pointer_tap); }

void Player::tick(std::span<NotNull<IDamageable*> const> targets, Seconds const dt) {
	auto const y_position = m_controller->tick(dt);
	set_y(y_position);

	auto const muzzle_position = get_muzzle_position();
	if (m_controller->is_firing() && m_debug.shots_remaining > 0) {
		if (auto round = m_weapon->fire(muzzle_position)) {
			m_weapon_rounds.push_back(std::move(round));
			--m_debug.shots_remaining;
		}
	}

	auto const round_state = IWeaponRound::State{.targets = targets, .muzzle_position = muzzle_position};
	for (auto const& round : m_weapon_rounds) { round->tick(round_state, dt); }
	std::erase_if(m_weapon_rounds, [](auto const& charge) { return charge->is_destroyed(); });

	m_weapon->tick(dt);

	foam_particles.set_position(get_exhaust_position());

	foam_particles.tick(dt);

	if (m_debug.shots_remaining <= 0) { debug_switch_weapon(); }
}

void Player::draw(Shader& shader) const {
	foam_particles.draw(shader);
	ship.draw(shader);

	for (auto const& round : m_weapon_rounds) { round->draw(shader); }
}

void Player::set_y(float const y) { ship.transform.position.y = y; }

auto Player::get_muzzle_position() const -> glm::vec2 { return ship.transform.position + 0.5f * glm::vec2{ship.get_shape().size.x, 0.0f}; }

auto Player::get_exhaust_position() const -> glm::vec2 { return ship.transform.position - 0.5f * glm::vec2{ship.get_shape().size.x, 0.0f}; }

void Player::set_controller(std::unique_ptr<IController> controller) {
	if (!controller) { return; }
	m_controller = std::move(controller);
}

void Player::do_inspect() {
	if constexpr (bave::imgui_v) {
		if (ImGui::TreeNodeEx("Controller", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			m_controller->inspect();
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Weapon", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			m_weapon->inspect();
			im_text("shots remaining: {}", m_debug.shots_remaining);
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Status", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) {
			health.inspect();
			ImGui::TreePop();
		}
	}
}

void Player::setup_ship() {
	auto const x = m_services->get<ILayout>().get_player_x();
	ship.transform.position.x = x;
	auto rounded_quad = RoundedQuad{};
	rounded_quad.size = glm::vec2{100.0f};
	rounded_quad.corner_radius = 20.0f;
	ship.set_shape(rounded_quad);

	auto const& style = m_services->get<Styles>();
	//m_style = style.player["default"];
}

void Player::setup_foam() {
	using Modifier = ParticleEmitter::Modifier;
	foam_particles.config.quad_size = glm::vec2{50.f};
	foam_particles.config.velocity.linear.angle = {Degrees{80.0f}, Degrees{100.0f}};
	foam_particles.config.velocity.linear.speed = {-360.0f, -270.0f};
	foam_particles.config.ttl = {2s, 3s};
	foam_particles.config.lerp.tint.hi.channels.w = 0xff;
	foam_particles.config.lerp.scale.hi = glm::vec2{0.85f};
	foam_particles.config.count = 80;
	foam_particles.modifiers = {Modifier::eTranslate, Modifier::eTint, Modifier::eScale};
	foam_particles.set_position(get_exhaust_position());
	foam_particles.pre_warm();
}

void Player::debug_switch_weapon() {
	if (m_weapon && !m_weapon->is_idle()) { return; }

	if (dynamic_cast<GunKinetic const*>(m_weapon.get()) != nullptr) {
		m_weapon = std::make_unique<GunBeam>(*m_services);
		m_debug.shots_remaining = 2;
		return;
	}

	m_weapon = std::make_unique<GunKinetic>(*m_services);
	m_debug.shots_remaining = 10;
}
} // namespace spaced
