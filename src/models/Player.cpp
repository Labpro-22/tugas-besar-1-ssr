#include "Player.hpp"
#include <algorithm>


Player::Player(std::string username, int playerIndex, int initialMoney)
    :   username(username), 
        playerIndex(playerIndex), 
        money(initialMoney), 
        position(0), 
        status(PlayerStatus::ACTIVE), 
        isShieldActive(false), 
        discount(0), 
        jailAttempts(0), 
        hasRolledDice(false), 
        hasUsedSkill(false), 
        consecutiveDoubles(0) {}


Player::~Player() { for (SkillCard* card : hand) delete card; hand.clear(); }


HumanPlayer::HumanPlayer(std::string username, int playerIndex, int initialMoney) : Player(username, playerIndex, initialMoney) {}
HumanPlayer::~HumanPlayer() {}
int HumanPlayer::chooseInput(std::vector<int> choices) { return choices.empty() ? -1 : choices[0]; }


/*
void Player::moveTo(int pos) { position = pos; }
int Player::moveForward(int steps, Board* board) {
    position = (position + steps) % board->getTileCount();
    return position;
}

void Player::addProperty(Property* property) { properties.push_back(property); }
void Player::removeProperty(Property* property) {
    properties.erase(std::remove(properties.begin(), properties.end(), property), properties.end());
}

void Player::addCard(SkillCard* card) { hand.push_back(card); }
SkillCard* Player::removeCard(int index) {
    if (index >= 0 && index < hand.size()) {
        SkillCard* c = hand[index];
        hand.erase(hand.begin() + index);
        return c;
    }
    return nullptr;
}

int Player::getCardCount() { return hand.size(); }
int Player::getTotalWealth() {
    int w = money;
    for (auto p : properties) w += p->getTotalValue();
    return w;
}

std::vector<Property*> Player::getPropertiesByColor(raylib::Color color) {
    std::vector<Property*> res;
    for (auto p : properties) {
        if (auto sp = dynamic_cast<StreetProperty*>(p)) {
            if (sp->colorGroup.r == color.r && sp->colorGroup.g == color.g && sp->colorGroup.b == color.b) res.push_back(p);
        }
    }
    return res;
}

int Player::getOwnedRailroadCount() { return std::count_if(properties.begin(), properties.end(), [](Property* p){ return p->getType() == "Railroad"; }); }
int Player::getOwnedUtilityCount() { return std::count_if(properties.begin(), properties.end(), [](Property* p){ return p->getType()  == "Utility"; }); }
bool Player::canAfford(int amount) { return money >= amount; }
int Player::getMaxLiquidationValue() { return getTotalWealth(); }
void Player::printProperties() {}
bool Player::isActive() { return status == PlayerStatus::ACTIVE; }
bool Player::isBankrupt() { return status == PlayerStatus::BANKRUPT; }
bool Player::isJailed() { return status == PlayerStatus::JAILED; }

BotPlayer::BotPlayer(std::string username, int playerIndex, int initialMoney) : Player(username, playerIndex, initialMoney) {}
BotPlayer::~BotPlayer() {}
int BotPlayer::chooseInput(std::vector<int> choices) { return choices.empty() ? -1 : choices[0]; }
*/