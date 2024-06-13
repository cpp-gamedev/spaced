#pragma once
#include <bave/driver.hpp>
#include <spaced/game_driver.hpp>
#include <spaced/services/services.hpp>

namespace spaced {
class Serializer;
struct Resources;
struct SceneSwitcher;

class Spaced : public GameDriver {
  public:
	explicit Spaced(bave::App& app);

  private:
	static void set_bindings(Serializer& serializer);

	void load_resources();
	void create_services();
	void set_layout();
	void set_prefs();
	void set_scene();

	bave::Logger m_log{"Spaced"};
	bave::Ptr<Resources> m_resources{};
};
} // namespace spaced
