# Nova Swarm

**A 2D space shooter game, homage to Space Impact**

[![Build Status](https://github.com/cpp-gamedev/spaced/actions/workflows/ci.yml/badge.svg)](https://github.com/cpp-gamedev/spaced/actions/workflows/ci.yml)

![Screenshot_20240706-190240](https://github.com/cpp-gamedev/spaced/assets/16272243/ca360512-43dc-4a18-a286-ed9d7f6f4171)

## Running the game

### Requirements

1. GPU that supports Vulkan 1.1+.
1. Vulkan 1.1+ loader (driver).
1. Windows:
    1. x64 (64 bit Intel/AMD).
    1. Windows 10+.
    1. Latest Visual C++ runtime.
1. GNU/Linux:
    1. x64 (64 bit Intel/AMD).
    1. libstdc++6 or newer.
1. MacOS (experimental):
    1. arm64 (64 bit ARM).
    1. Vulkan SDK must be installed globally.
1. Android:
    1. arm64 (64 bit ARM) or x64 (64 bit Intel/AMD).

*Note: Some GPUs on Android seem to have driver bugs that cause rendering artifacts and can sometimes crash the app.*

### Steps

1. Download the latest release from the [releases page](https://github.com/cpp-gamedev/spaced/releases).
2. Unzip to a desired location / install the APK.
3. Run the app.

*Note: executable binaries in releases are not signed.*

## Building the source

### Requirements

In addition to runtime requirements:

1. C++20 compiler and standard library.
1. CMake 3.22+.
1. Git.

### Steps

1. Clone this repo locally.
1. Use an IDE / the command line to choose a compatible CMake preset or configure a build manually. First-time configuration takes some time to complete.
1. Build and run/debug.

## Attribution

See [attribution.txt](attribution.txt).

## Contributing

Contributions are welcome via PRs.
