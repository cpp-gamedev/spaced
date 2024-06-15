#include <spaced/game/tiled_bg.hpp>

namespace spaced {
using bave::RenderInstance;
using bave::Seconds;
using bave::Services;
using bave::Shader;

TiledBg::TiledBg(Services const& services) : m_layout(&services.get<Layout>()) {}

void TiledBg::set_tile_size(glm::vec2 const size) {
	auto shape = m_quad.get_shape();
	shape.size = size;
	m_quad.set_shape(shape);

	auto const cols = static_cast<int>(m_layout->world_space.x / size.x) + 1;
	auto const rows = static_cast<int>(m_layout->world_space.y / size.y) + 1;
	auto const lt = 0.5f * glm::vec2{-m_layout->world_space.x + size.x, m_layout->world_space.y - size.y};

	m_exit_x = lt.x - size.x;

	m_quad.instances.clear();
	auto instance = RenderInstance{};
	instance.transform.position = lt;
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols + 1; ++col) {
			m_quad.instances.push_back(instance);
			instance.transform.position.x += size.x;
		}
		instance.transform.position.x = lt.x;
		instance.transform.position.y -= size.y;
	}
}

void TiledBg::tick(Seconds const dt) {
	if (x_speed <= 0.0f) { return; }

	auto const dx = x_speed * dt.count();
	auto max_x = 0.0f;
	for (auto& instance : m_quad.instances) {
		instance.transform.position.x -= dx;
		max_x = std::max(max_x, instance.transform.position.x);
	}
	for (auto& instance : m_quad.instances) {
		if (instance.transform.position.x <= m_exit_x) { instance.transform.position.x = max_x + m_quad.get_shape().size.x; }
	}
}

void TiledBg::draw(Shader& shader) const {
	static bool s_wf{false};
	if (s_wf) { shader.polygon_mode = vk::PolygonMode::eLine; }
	m_quad.draw(shader);
	if (s_wf) { shader.polygon_mode = vk::PolygonMode::eFill; }
}
} // namespace spaced
