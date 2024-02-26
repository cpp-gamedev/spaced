#pragma once
#include <spaced/scene.hpp>
#include <spaced/services/service.hpp>

namespace spaced {
class ISceneSwitcher : public IService {
  public:
	virtual void switch_to(std::unique_ptr<Scene> new_scene) = 0;
};
} // namespace spaced
