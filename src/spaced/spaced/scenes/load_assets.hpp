#pragma once
#include <spaced/scene.hpp>

namespace spaced {
class LoadAssets : public Scene {
  public:
	explicit LoadAssets(bave::App& app, Services const& services);

  private:
	auto build_load_stages() -> std::vector<AsyncExec::Stage> final;
	void on_loaded() final;
};
} // namespace spaced
