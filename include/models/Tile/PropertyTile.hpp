#pragma once

#include "Tile.hpp"

class Game;
class Player;
class Property;

class PropertyTile : public Tile {
public:
	Property* property;

	PropertyTile();
	PropertyTile(int index, const std::string& code, const std::string& name, const std::string& category, Property* property);
	~PropertyTile() override = default;

	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};
