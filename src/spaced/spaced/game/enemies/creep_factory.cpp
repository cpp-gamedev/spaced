#include <spaced/game/enemies/creep.hpp>
#include <spaced/game/enemies/creep_factory.hpp>

namespace spaced {
auto CreepFactory::spawn_enemy() -> std::unique_ptr<Enemy> { return std::make_unique<Creep>(get_services()); }
} // namespace spaced
