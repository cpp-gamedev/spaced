#pragma once
#include <spaced/scene.hpp>
#include <spaced/services/service.hpp>

namespace spaced {
class ISceneSwitcher : public IService {
  public:
	virtual void switch_to_scene(std::unique_ptr<Scene> new_scene) = 0;

	template <std::derived_from<Scene> T, typename... Args>
		requires(std::constructible_from<T, bave::App&, Services const&, Args && ...>)
	void switch_to(Args&&... args) {
		switch_to_scene(std::make_unique<T>(get_app(), get_services(), std::forward<Args>(args)...));
	}

  private:
	[[nodiscard]] virtual auto get_app() const -> bave::App& = 0;
	[[nodiscard]] virtual auto get_services() const -> Services const& = 0;
};
} // namespace spaced
