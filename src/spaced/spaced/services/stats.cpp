#include <djson/json.hpp>
#include <spaced/services/stats.hpp>

namespace spaced {
void Stats::from(dj::Json const& json) {
	from_json(json["run_count"], run_count);

	auto const& in_game = json["game"];
	from_json(in_game["play_count"], game.play_count);

	auto const& in_player = json["player"];
	from_json(in_player["powerups_collected"], player.powerups_collected);
	from_json(in_player["shots_fired"], player.shots_fired);
	from_json(in_player["enemies_poofed"], player.enemies_poofed);
	from_json(in_player["death_count"], player.death_count);
}

void Stats::to(dj::Json& out) const {
	to_json(out["run_count"], run_count);

	auto& out_game = out["game"];
	to_json(out_game["play_count"], game.play_count);

	auto& out_player = out["player"];
	to_json(out_player["powerups_collected"], player.powerups_collected);
	to_json(out_player["shots_fired"], player.shots_fired);
	to_json(out_player["enemies_poofed"], player.enemies_poofed);
	to_json(out_player["death_count"], player.death_count);
}
} // namespace spaced
