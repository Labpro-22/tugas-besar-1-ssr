#pragma once

#include <sstream>
#include <string>

class Game;
class Player;

class Tile {
public:
	int index;
	std::string code;
	std::string name;
	std::string category;

	Tile();
	Tile(int index, const std::string& code, const std::string& name, const std::string& category);
	virtual ~Tile() = default;

protected:
	virtual void onLanded(Player* player, Game* game) = 0;
	virtual void getDisplayInfo(std::stringstream& output) const = 0;
	virtual void getTileType(std::stringstream& output) const = 0;
};
