#pragma once

#include <string>
#include <vector>
#include "Board.hpp"
#include "Property.hpp"
#include "Card.hpp"
#include <raylib-cpp.hpp>
using namespace std;

class Board;
class Property;

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
    vector<Property*> &getAllProperties()  { return properties; }
    vector<Property*> getPropertiesByColor(raylib::Color color);
    int getOwnedRailroadCount();
    int getOwnedUtilityCount();

    void setStatus(PlayerStatus status) { status = status; }
    void setJailAttempts(int attempt) { jailAttempts = attempt; }

    void incConsecutiveDouble();
    void resetConsecutiveDouble();
    bool aboveSpeedLimit();

    void moveTo(Board* board, int position);
    void moveForward(Board* board, int steps);

    void addProperty(Property* property);
    void removeProperty(Property* property);

    void addCard(SkillCard* card);
    SkillCard* removeCard(int index);

    void printProperties();

    bool isActive();
    bool isBankrupt();
    bool isJailed();
    
    bool canAfford(int amount) { return money >= amount; };
    void addMoney(int amount) { money += amount; };
    void deductMoney(int amount) { money -= amount; };
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