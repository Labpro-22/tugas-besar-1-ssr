#pragma once

#include "Tile.hpp"

class Game;
class Player;

class JailTile : public Tile {
public:
	JailTile();
	JailTile(int index, const std::string& code, const std::string& name, const std::string& category);
	~JailTile() override = default;

	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};
