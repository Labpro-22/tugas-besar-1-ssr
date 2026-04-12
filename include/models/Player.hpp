#pragma once

#include <string>
#include <vector>
#include "Board.hpp"
#include "Property.hpp"
#include "SkillCard.hpp"
#include "raylib.h"

enum class PlayerStatus {
    ACTIVE,
    BANKRUPT,
    JAILED
};

class Player{
Protected:
    std::stringusername;
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

    virtual int chooseInput(std::vector<int> choices) = 0;
};