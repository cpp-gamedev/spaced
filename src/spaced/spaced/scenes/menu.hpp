#pragma once
#include <spaced/scenes/base.hpp>

namespace spaced {
class MenuScene : public BaseScene {
  public:
	explicit MenuScene(bave::App& app, bave::Services const& services);

  private:
	auto get_asset_manifest() -> bave::AssetManifest final;
	void on_loaded() final;

	void create_ui();
};
} // namespace spaced
