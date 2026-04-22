#pragma once

#include <sstream>
#include <string>
#include <algorithm>
#include <random>
#include <type_traits>

#include "Game.hpp"
#include "Player.hpp"

class Player;
class Game;

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



class TaxTile : public Tile {
public:
	enum TaxType {
		TAX_PPH,
		TAX_PBM
	};

	TaxType taxType;
	int flatAmount;
	float percentage;

	TaxTile();
	TaxTile(int index, const std::string& code, const std::string& name, const std::string& category, TaxType taxType, int flatAmount, float percentage);
	~TaxTile() override = default;

	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};



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



class JailTile : public Tile {
public:
	JailTile();
	JailTile(int index, const std::string& code, const std::string& name, const std::string& category);
	~JailTile() override = default;

	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};



class GoToJailTile : public Tile {
public:
	GoToJailTile();
	GoToJailTile(int index, const std::string& code, const std::string& name, const std::string& category);
	~GoToJailTile() override = default;

	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};



class FreeParkingTile : public Tile {
public:
	FreeParkingTile();
	FreeParkingTile(int index, const std::string& code, const std::string& name, const std::string& category);
	~FreeParkingTile() override = default;

	void onLanded(Player* player, Game* game) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};



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



