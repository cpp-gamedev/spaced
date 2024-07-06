#include <bave/clap/clap.hpp>
#include <bave/desktop_app.hpp>
#include <spaced/build_version.hpp>
#include <spaced/game_title.hpp>
#include <spaced/spaced.hpp>
#include <iostream>

namespace {
using bave::App;
using bave::DataLoaderBuilder;
using bave::DesktopApp;
using bave::to_string;

namespace {
auto get_log_filename() {
	auto ret = std::string{};
	ret.reserve(spaced::game_title_v.size() + 4);
	for (char const c : spaced::game_title_v) {
		if (std::isspace(static_cast<unsigned char>(c)) != 0) { continue; }
		ret += c;
	}
	ret += ".log";
	return ret;
}
} // namespace

auto parse_args(int const argc, char const* const* argv) -> std::optional<int> {
	namespace clap = bave::clap;
	auto name = clap::make_app_name(*argv);
	auto description = std::string{"2D platformer game using bave"};
	auto version = to_string(spaced::build_version_v);
	auto options = clap::Options{std::move(name), std::move(description), std::move(version)};

	auto show_bave_version = false;
	options.flag(show_bave_version, "bave-version", "show bave version");

	auto const result = options.parse(argc, argv);
	if (clap::should_quit(result)) { return clap::return_code(result); }

	if (show_bave_version) {
		std::cout << "bave version " << to_string(bave::build_version_v) << "\n";
		return EXIT_SUCCESS;
	}

	return {};
}

void set_log_max_levels() {
	using Level = bave::log::Level;
	static constexpr auto warn_v = std::array{
		"DesktopApp", "DataStore", "Vulkan", "RenderDevice", "ImageCache", "BufferCache", "ShaderCache", "PipelineCache",
	};

	auto const set = [](auto const& names, Level const level) {
		for (auto const* name : names) { bave::log::set_max_level(name, level); }
	};

	set(warn_v, Level::eWarn);
}

auto run(int const argc, char const* const* argv) -> int {
	auto data_loader_builder = DataLoaderBuilder{argc, argv}.add_zip("assets.zip").add_dir("assets");
	auto create_info = DesktopApp::CreateInfo{
		.title = spaced::game_title_v.data(),
		.mode = bave::Windowed{.extent = {1920, 1080}},
		.msaa = vk::SampleCountFlagBits::e4,
		.data_loader = data_loader_builder.build(),
		.log_filename = get_log_filename(),
	};

	set_log_max_levels();

	auto app = DesktopApp{std::move(create_info)};
	app.set_bootloader([](App& app) { return std::make_unique<spaced::Spaced>(app); });
	return static_cast<int>(app.run());
}
} // namespace

auto main(int argc, char** argv) -> int {
	if (auto const ret = parse_args(argc, argv)) { return *ret; }
	return run(argc, argv);
}
