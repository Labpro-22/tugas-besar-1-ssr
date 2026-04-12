#pragma once

#include "Tile.hpp"

class Game;
class Player;

class FestivalTile : public Tile {
public:
	float multiplier;
	int duration;

	FestivalTile();
	FestivalTile(int index, const std::string& code, const std::string& name, const std::string& category, float multiplier, int duration);
	~FestivalTile() override = default;

	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};
