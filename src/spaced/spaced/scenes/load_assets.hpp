#pragma once
#include <bave/scene.hpp>

namespace spaced {
class LoadAssets : public bave::Scene {
  public:
	explicit LoadAssets(bave::App& app, bave::Services const& services);

  private:
	auto build_load_stages() -> std::vector<bave::AsyncExec::Stage> final;
	void on_loaded() final;
};
} // namespace spaced
