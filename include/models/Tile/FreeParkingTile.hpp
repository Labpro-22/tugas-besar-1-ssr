#pragma once

#include "Tile.hpp"

class Game;
class Player;

class FreeParkingTile : public Tile {
public:
	FreeParkingTile();
	FreeParkingTile(int index, const std::string& code, const std::string& name, const std::string& category);
	~FreeParkingTile() override = default;

	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};
