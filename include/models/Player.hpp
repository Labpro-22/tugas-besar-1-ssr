#pragma once

#include <string>
#include <vector>
#include <raylib-cpp.hpp>

class Board;
class Property;
class SkillCard;

enum class PlayerStatus {
    ACTIVE,
    BANKRUPT,
    JAILED
};

class Player {
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
    int consecutiveDoubles;

public:
    bool hasRolledDice;
    bool hasUsedSkill;

    Player(std::string username, int playerIndex, int initialMoney);
    virtual ~Player();

    // GETTER
    int getPlayerIndex(){ return playerIndex;}
    std::string getUsername(){ return username;}
    int getMoney(){ return money;}
    int getPosition(){ return position;}
    PlayerStatus getStatus(){ return status;}
    int getCardCount();
    std::vector<Property*> &getAllProperties()  { return properties; }
    std::vector<Property*> getPropertiesByColor(raylib::Color color);
    int getOwnedRailroadCount();
    int getOwnedUtilityCount();

    void setStatus(PlayerStatus status) { this->status = status; }
    void setPosition(int pos) { position = pos; }
    int getJailAttempts() { return jailAttempts; }
    void setJailAttempts(int attempt) { jailAttempts = attempt; }
    void incJailAttempts() { jailAttempts++; }

    // Operator Overloading
    Player& operator+=(int amount);
    Player& operator-=(int amount);
    bool operator<(Player& other);
    bool operator>(Player& other);

    void incConsecutiveDouble();
    void resetConsecutiveDouble();
    bool aboveSpeedLimit();

    void moveTo(Board* board, int position);
    void moveForward(Board* board, int steps);

    void addProperty(Property* property);
    void removeProperty(Property* property);

    void addCard(SkillCard* card);
    SkillCard* removeCard(int index);
    std::vector<SkillCard*>& getHand() { return hand; }

    void printProperties();

    bool isActive();
    bool isBankrupt();
    bool isJailed();
    
    bool canAfford(int amount) { return money >= amount; };
    void addMoney(int amount) { money += amount; };
    void deductMoney(int amount) { money -= amount; };
    int getTotalWealth();
    int getMaxLiquidationValue();
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
