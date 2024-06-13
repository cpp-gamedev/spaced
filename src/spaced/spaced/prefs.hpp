#pragma once
#include <bave/app.hpp>
#include <bave/services/audio.hpp>
#include <bave/ui/view.hpp>

namespace spaced {
struct Prefs {
	class View;

	float music_gain{1.0f};
	float sfx_gain{1.0f};

	static auto load(bave::App const& app) -> Prefs;
	void save(bave::App const& app) const;
};

class Prefs::View : public bave::ui::View {
  public:
	View(View const&) = delete;
	View(View&&) = delete;
	auto operator=(View const&) = delete;
	auto operator=(View&&) = delete;

	explicit View(bave::NotNull<bave::App const*> app, bave::Services const& services);
	~View() override;

  private:
	bave::NotNull<bave::App const*> m_app;
	bave::NotNull<bave::IAudio*> m_audio;
	Prefs m_prefs;
};
} // namespace spaced
