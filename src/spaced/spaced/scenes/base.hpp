#pragma once
#include <bave/scene.hpp>

namespace spaced {
class BaseScene : public bave::Scene {
  protected:
	explicit BaseScene(bave::App& app, bave::Services const& services, std::string tag) : Scene(app, services, std::move(tag)) {
		m_shader = {
			.vert = "assets/shaders/default.vert",
			.frag = "assets/shaders/default.frag",
		};
	}
};
} // namespace spaced
