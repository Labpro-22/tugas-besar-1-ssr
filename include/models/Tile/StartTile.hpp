#pragma once

#include "Tile.hpp"

class Game;
class Player;

class StartTile : public Tile {
public:
	StartTile();
	StartTile(int index, const std::string& code, const std::string& name, const std::string& category);
	~StartTile() override = default;

	void onPassed(Player* player, Game* game);
	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};
