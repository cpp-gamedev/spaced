#pragma once
#include <bave/app.hpp>
#include <spaced/ui/view.hpp>

namespace spaced {
class IAudio;

struct Prefs {
	class View;

	float music_gain{1.0f};
	float sfx_gain{1.0f};

	static auto load(bave::App const& app) -> Prefs;
	void save(bave::App const& app) const;
};

class Prefs::View : public ui::View {
  public:
	View(View const&) = delete;
	View(View&&) = delete;
	auto operator=(View const&) = delete;
	auto operator=(View&&) = delete;

	explicit View(bave::NotNull<bave::App const*> app, Services const& services);
	~View() override;

  private:
	bave::NotNull<bave::App const*> m_app;
	bave::NotNull<IAudio*> m_audio;
	Prefs m_prefs;
};
} // namespace spaced
