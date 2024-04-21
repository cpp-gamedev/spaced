#pragma once
#include <string>
#include <vector>

namespace spaced {
struct AssetManifest {
	std::vector<std::string> textures{};
	std::vector<std::string> mip_mapped_textures{};
	std::vector<std::string> fonts{};
	std::vector<std::string> audio_clips{};
	std::vector<std::string> particle_emitters{};
};
} // namespace spaced
