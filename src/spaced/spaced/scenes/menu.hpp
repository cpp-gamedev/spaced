#pragma once
#include <bave/scene.hpp>

namespace spaced {
class MenuScene : public bave::Scene {
  public:
	explicit MenuScene(bave::App& app, bave::Services const& services);

  private:
	auto build_load_stages() -> std::vector<bave::AsyncExec::Stage> final;
	void on_loaded() final;

	void create_ui();
};
} // namespace spaced
