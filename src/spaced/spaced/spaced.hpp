#pragma once
#include <bave/driver.hpp>
#include <bave/game_driver.hpp>

namespace bave {
struct Resources;
}

namespace spaced {
class Serializer;
struct SceneSwitcher;

class Spaced : public bave::GameDriver {
  public:
	explicit Spaced(bave::App& app);

  private:
	static void set_bindings(Serializer& serializer);

	void save_styles();
	void create_services();
	void set_layout();
	void set_prefs();
	void set_scene();

	bave::Logger m_log{"Spaced"};
	bave::Ptr<bave::Resources> m_resources{};
};
} // namespace spaced
