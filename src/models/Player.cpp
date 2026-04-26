#include "Player.hpp"
#include "Board.hpp"
#include "Tile.hpp"
#include "Property.hpp"
#include "Card.hpp"
#include <algorithm>
#include <iostream>


Player::Player(std::string username, int playerIndex, int initialMoney) :   
    username(username), 
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


Player::~Player() { 
    for(SkillCard *card : hand) delete card; 
    hand.clear(); 
}


void Player::incConsecutiveDouble(){ consecutiveDoubles++; }

void Player::resetConsecutiveDouble(){ consecutiveDoubles = 0; }

bool Player::aboveSpeedLimit(){ return consecutiveDoubles >= 3;}



void Player::moveTo(Board* board, int pos){ 
    position = pos;
}


void Player::moveForward(Board* board, int steps) {
    position = (position + steps) % board->getTileCount();
}


void Player::addProperty(Property* property){ 
    properties.push_back(property); 
}


void Player::removeProperty(Property* property){
    properties.erase(std::remove(properties.begin(), properties.end(), property), properties.end());
}


void Player::addCard(SkillCard* card) { 
    hand.push_back(card); 
}


SkillCard* Player::removeCard(int index) {
    if (index >= 0 && index < hand.size()) {
        SkillCard* c = hand[index];
        hand.erase(hand.begin() + index);
        return c;
    }
    return nullptr;
}


int Player::getCardCount() { 
    return hand.size(); 
}


int Player::getTotalWealth() {
    int w = money;
    for (auto p : properties) w += p->getTotalValue();
    return w;
}


std::vector<Property*> Player::getPropertiesByColor(raylib::Color color) {
    std::vector<Property*> res;
    for (auto p : properties) {
        if (auto sp = dynamic_cast<StreetProperty*>(p)) {
            if (sp->getColorGroup().r == color.r && sp->getColorGroup().g == color.g && sp->getColorGroup().b == color.b) res.push_back(p);
        }
    }
    return res;
}


int Player::getOwnedRailroadCount() { 
    return std::count_if(properties.begin(), properties.end(), 
        [](Property* p){ return p->getType() == "Railroad"; }
    );
}


int Player::getOwnedUtilityCount() { 
    return std::count_if(properties.begin(), properties.end(), 
        [](Property* p){ return p->getType()  == "Utility"; }
    ); 
}


bool Player::isActive() { return status == PlayerStatus::ACTIVE; }
bool Player::isBankrupt() { return status == PlayerStatus::BANKRUPT; }
bool Player::isJailed() { return status == PlayerStatus::JAILED; }

Player& Player::operator+=(int amount) {
    addMoney(amount);
    return *this;
}

Player& Player::operator-=(int amount) {
    deductMoney(amount);
    return *this;
}

bool Player::operator<(Player& other) {
    return getTotalWealth() < other.getTotalWealth();
}

bool Player::operator>(Player& other) {
    return getTotalWealth() > other.getTotalWealth();
}

void Player::printProperties() {
    std::cout << "--- PROPERTI MILIK " << username << " ---\n";
    if (properties.empty()) {
        std::cout << "(Belum memiliki properti)\n";
        return;
    }
    for (auto p : properties) {
        std::cout << "- [" << p->getCode() << "] " << p->getName() << " | Status: ";
        switch(p->getStatus()) {
            case PropertyStatus::OWNED: std::cout << "OWNED"; break;
            case PropertyStatus::MORTGAGED: std::cout << "MORTGAGED"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        if (auto* sp = dynamic_cast<StreetProperty*>(p)) {
            std::cout << " | Bangunan: " << sp->getBuildingCount();
        }
        std::cout << "\n";
    }
}


HumanPlayer::HumanPlayer(std::string username, int playerIndex, int initialMoney) : Player(username, playerIndex, initialMoney) {}
HumanPlayer::~HumanPlayer() {}

int HumanPlayer::chooseInput(std::vector<int> choices) { 
    if (choices.empty()) return -1;
    
    std::cout << "Pilihan yang tersedia: ";
    for (size_t i = 0; i < choices.size(); ++i) {
        std::cout << choices[i] << (i == choices.size() - 1 ? "" : ", ");
    }
    std::cout << "\nMasukkan pilihan: ";
    
    int choice;
    while (true) {
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "Input tidak valid. Masukkan angka pilihan: ";
            continue;
        }
        
        bool found = false;
        for (int c : choices) {
            if (c == choice) {
                found = true;
                break;
            }
        }
        
        if (found) {
            std::cin.ignore(1000, '\n');
            return choice;
        } else {
            std::cout << "Pilihan tidak valid. Silakan coba lagi: ";
        }
    }
}


BotPlayer::BotPlayer(std::string username, int playerIndex, int initialMoney) : Player(username, playerIndex, initialMoney) {}
BotPlayer::~BotPlayer() {}

int BotPlayer::chooseInput(std::vector<int> choices) { return choices.empty() ? -1 : choices[0]; }
