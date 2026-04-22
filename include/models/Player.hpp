#pragma once

#include <string>
#include <vector>
#include "Board.hpp"
#include "Property.hpp"
#include "Card.hpp"
#include <raylib-cpp.hpp>

class Board;

enum class PlayerStatus {
    ACTIVE,
    BANKRUPT,
    JAILED
};



class Player{
protected:
    std::string username;
    int money;
    int position;
    PlayerStatus status;
    std::vector<Property*> properties;
    std::vector<SkillCard*> hand;
    int playerIndex;
    bool isShieldActive;
    int discount;
    int jailAttempts;
    bool hasRolledDice;
    bool hasUsedSkill;
    int consecutiveDoubles;

public:
    Player(std::string username, int playerIndex, int initialMoney);
    virtual ~Player();

    void moveTo(int position);
    int moveForward(int steps, Board* board);
    void addProperty(Property* property);
    void removeProperty(Property* property);
    void addCard(SkillCard* card);
    SkillCard* removeCard(int index);
    int getCardCount();
    int getTotalWealth();
    std::vector<Property*> getPropertiesByColor(raylib::Color color);
    int getOwnedRailroadCount();
    int getOwnedUtilityCount();
    bool canAfford(int amount);
    int getMaxLiquidationValue();
    void printProperties();
    bool isActive();
    bool isBankrupt();
    bool isJailed();
    bool tryLiquidateAsset();

    virtual int chooseInput(std::vector<int> choices) = 0;
};



class HumanPlayer : public Player {
public:
    HumanPlayer(std::string username, int playerIndex, int initialMoney);
    ~HumanPlayer();

    int chooseInput(std::vector<int> choices) override;
};



class BotPlayer : public Player {
public:
    BotPlayer(std::string username, int playerIndex, int initialMoney);
    ~BotPlayer();
    
    int chooseInput(std::vector<int> choices) override;
};