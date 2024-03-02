#include <glm/gtx/norm.hpp>
#include <spaced/game/damage.hpp>

namespace spaced {

void Damage::inflict(float amount) { health -= amount; }

bool Damage::is_dead() const { return dead; }

void Damage::update() {
	dead = health <= 0;
	if (dead) { health = 0; }
}
} // namespace spaced
