#include <bave/android_app.hpp>
#include <spaced/spaced.hpp>

extern "C" {
// Android entry point: must have C linkage.
void android_main(android_app* andr_app) {
	// we will use bave::AndroidApp.
	// create the App instance.
	auto app = bave::AndroidApp{*andr_app, vk::SampleCountFlagBits::e4};
	// setup the entry point (Flappy).
	app.set_bootloader([](bave::App& app) { return std::make_unique<spaced::Spaced>(app); });
	// run App and return its exit code.
	// after its own setup, App will create Flappy (via the factory passed above) and drive it every frame.
	app.run();
}
}
