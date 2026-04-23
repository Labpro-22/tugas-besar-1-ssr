#pragma once

#include <string>
#include <vector>
#include "Board.hpp"
#include "Property.hpp"
#include "Card.hpp"
#include <raylib-cpp.hpp>
using namespace std;


enum class PlayerStatus {
    ACTIVE,
    BANKRUPT,
    JAILED
};



class Player{
protected:
    string username;
    int money;
    int position;
    PlayerStatus status;
    vector<Property*> properties;
    vector<SkillCard*> hand;
    int playerIndex;
    bool isShieldActive;
    int discount;
    int jailAttempts;
    bool hasRolledDice;
    bool hasUsedSkill;
    int consecutiveDoubles;

public:
    Player(string username, int playerIndex, int initialMoney);
    virtual ~Player();

    // GETTER
    int getPlayerIndex(){ return playerIndex;}
    string getUsername(){ return username;}
    int getMoney(){ return money;}
    int getCardCount();
    vector<Property*> getPropertiesByColor(raylib::Color color);
    int getOwnedRailroadCount();
    int getOwnedUtilityCount();

    void moveTo(int position);
    int moveForward(int steps, Board* board);
    void addProperty(Property* property);
    void removeProperty(Property* property);
    void addCard(SkillCard* card);
    SkillCard* removeCard(int index);
    void printProperties();
    bool isActive();
    bool isBankrupt();
    bool isJailed();
    
    /*====Financial====*/
    bool canAfford(int amount);
    void addMoney(int amount);  // (salary, rent, dll.)
    void deductMoney(int amount);   // pay money; clamps to 0 if insufficient (bankruptcy handled by caller)
    int getTotalWealth();
    int getMaxLiquidationValue();
    virtual int chooseInput(vector<int> choices) = 0;
};



class HumanPlayer : public Player {
public:
    HumanPlayer(string username, int playerIndex, int initialMoney);
    ~HumanPlayer();

    int chooseInput(vector<int> choices) override;
};



class BotPlayer : public Player {
public:
    BotPlayer(string username, int playerIndex, int initialMoney);
    ~BotPlayer();
    
    int chooseInput(vector<int> choices) override;
};