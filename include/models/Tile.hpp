#pragma once

#include <sstream>
#include <string>

#include "Player.hpp"
#include "GameSession.hpp"
#include "Property.hpp"
#include "Card.hpp"

class Player;
class GameSession;
class Property;
class Card;

enum class TileType {
	START,
	TAX,
	PROPERTY,
	JAIL,
	GO_TO_JAIL,
	FREE_PARKING,
	FESTIVAL,
	CARD
};

class Tile {
public:
	int index;
	std::string code;
	std::string name;
	std::string category;

	Tile();
	Tile(int index, const std::string& code, const std::string& name, const std::string& category);
	virtual ~Tile() = default;

	std::string &getName() { return name; }
	
	virtual void onLanded(Player* player) = 0;
	virtual void onPassed(Player* player) = 0;
	virtual void getDisplayInfo(std::stringstream& output) const = 0;
	virtual void getTileType(std::stringstream& output) const = 0;

	virtual TileType getType() const = 0;
	virtual class PropertyTile* asPropertyTile() { return nullptr; }
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

	TileType getType() const override { return TileType::TAX; }
	void onLanded(Player* player) override;
	void onPassed(Player* player) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;

	TaxType getTaxType() { return taxType; }
	int getFlatAmount() { return flatAmount; }
	float getPercentage() { return percentage; }
};



class StartTile : public Tile {
private:
	int salary;

public:
	StartTile();
	StartTile(int index, const std::string& code, const std::string& name, const std::string& category, int salary);
	~StartTile() override = default;

	TileType getType() const override { return TileType::START; }
	void onLanded(Player* player) override;
	void onPassed(Player* player) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};



class PropertyTile : public Tile {
public:
	Property* property;

	PropertyTile();
	PropertyTile(int index, const std::string& code, const std::string& name, const std::string& category, Property* property);
	~PropertyTile() override = default;

	TileType getType() const override { return TileType::PROPERTY; }
	PropertyTile* asPropertyTile() override { return this; }
	void onLanded(Player* player) override;
	void onPassed(Player* player) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};



class JailTile : public Tile {
public:
	JailTile();
	JailTile(int index, const std::string& code, const std::string& name, const std::string& category);
	~JailTile() override = default;

	TileType getType() const override { return TileType::JAIL; }
	void onLanded(Player* player) override;
	void onPassed(Player* player) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};



class GoToJailTile : public Tile {
public:
	GoToJailTile();
	GoToJailTile(int index, const std::string& code, const std::string& name, const std::string& category);
	~GoToJailTile() override = default;

	TileType getType() const override { return TileType::GO_TO_JAIL; }
	void onLanded(Player* player) override;
	void onPassed(Player* player) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};



class FreeParkingTile : public Tile {
public:
	FreeParkingTile();
	FreeParkingTile(int index, const std::string& code, const std::string& name, const std::string& category);
	~FreeParkingTile() override = default;

	TileType getType() const override { return TileType::FREE_PARKING; }
	void onLanded(Player* player) override;
	void onPassed(Player* player) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;
};



class FestivalTile : public Tile {
public:
	float multiplier;
	int duration;

	FestivalTile();
	FestivalTile(int index, const std::string& code, const std::string& name, const std::string& category, int multiplier, int duration);
	~FestivalTile() override = default;

	TileType getType() const override { return TileType::FESTIVAL; }
	void onLanded(Player* player) override;
	void onPassed(Player* player) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;

	float getMultiplier() { return multiplier; }
	int getDuration() { return duration; }
};



class CardTile : public Tile {
public:
	enum ActionType {FUND, OPPOTURNITY};

	CardTile();
	CardTile(int index, const std::string& code, const std::string& name, const std::string& category, ActionType actionType);
	~CardTile() override = default;

	TileType getType() const override { return TileType::CARD; }
	void onLanded(Player* player) override;
	void onPassed(Player* player) override;
	void getDisplayInfo(std::stringstream& output) const override;
	void getTileType(std::stringstream& output) const override;

private:
	ActionType actionType;
};
