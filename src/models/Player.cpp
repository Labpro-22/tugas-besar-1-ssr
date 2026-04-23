#include "Player.hpp"
#include <algorithm>


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

    Tile *tile = board->getTile(position);
    std::cout << "Pemain mendarat di " << tile->getName() << "...\n";
    tile->onLanded(this, nullptr);

}


void Player::moveForward(Board* board, int steps) {

    Tile *tile = board->getTile(position);
    std::cout << "Pemain mendarat di " << tile->getName() << "...\n";
    tile->onLanded(this, nullptr);

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


HumanPlayer::HumanPlayer(std::string username, int playerIndex, int initialMoney) : Player(username, playerIndex, initialMoney) {}
HumanPlayer::~HumanPlayer() {}

int HumanPlayer::chooseInput(std::vector<int> choices) { return choices.empty() ? -1 : choices[0]; }


BotPlayer::BotPlayer(std::string username, int playerIndex, int initialMoney) : Player(username, playerIndex, initialMoney) {}
BotPlayer::~BotPlayer() {}

int BotPlayer::chooseInput(std::vector<int> choices) { return choices.empty() ? -1 : choices[0]; }