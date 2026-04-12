#pragma once

#include "Tile.hpp"

class Card;
class Game;
class Player;

class CardTile : public Tile {
public:
	Card* card;

	CardTile();
	CardTile(int index, const std::string& code, const std::string& name, const std::string& category, Card* card);
	~CardTile() override = default;

	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};
