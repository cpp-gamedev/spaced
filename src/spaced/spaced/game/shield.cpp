#include <bave/services/resources.hpp>
#include <spaced/game/shield.hpp>

namespace spaced {
using bave::Resources;
using bave::Rgba;
using bave::Seconds;
using bave::Services;
using bave::Shader;
using bave::Texture;

Shield::Shield(Services const& services) {
	if (auto const texture = services.get<Resources>().get<Texture>("images/shield.png")) {
		m_sprite.set_texture(texture);
		m_sprite.set_size(texture->get_size());
	}
}

void Shield::tick(Seconds const dt) {
	if (!is_active()) { return; }

	ttl -= dt;

	auto const alpha = [&] {
		if (ttl < fadeout) { return ttl / fadeout; }
		return 1.0f;
	}();
	m_sprite.tint.channels.w = Rgba::to_u8(alpha);
}

void Shield::draw(Shader& shader) const {
	if (!is_active()) { return; }
	m_sprite.draw(shader);
}
} // namespace spaced
