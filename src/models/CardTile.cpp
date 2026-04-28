#include "Tile.hpp"

#include "GameApp.hpp"
#include "AppException.hpp"
#include "Card.hpp"
#include "GameSession.hpp"
#include <iostream>
#include <chrono>
#include <thread>

CardTile::CardTile() : Tile() {}

CardTile::CardTile(int index, const std::string& code, const std::string& name, const std::string& category, CardTile::ActionType actionType)
    : Tile(index, code, name, category), actionType(actionType) {}

void CardTile::onLanded(Player* player) {
    if (player == nullptr) {
        throw GameException("CardTile", "Player cannot be null.");
    }
    
    GameSession *game = GameApp::currentSession;
    Board *board = game->getBoard();

    board->printBoard();
    std::cout << "Pemain " << player->getUsername() << " mendarat di petak " << this->getName() << '\n';
    
    if(actionType == FUND){
        ActionCard *c = game->drawFundCard(false);
        std::cout << "Pemain " << player->getUsername() << " mendapatkan kartu dana umum, pemain harus " << c->actionName << '\n';
        c->execute(player);
        game->getFundDeck().shuffle();
    }
    else {
        ActionCard *c = game->drawOppoturnityCard(false);
        std::cout << "Pemain " << player->getUsername() << " mendapatkan kartu kesempatan, pemain harus " << c->actionName << '\n';
        c->execute(player);
        game->getOppoturnityDeck().shuffle();
    }
    
}

void CardTile::onPassed(Player* player) {
    if (player == nullptr) {
        throw GameException("CardTile", "Player cannot be null.");
    }

    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " melewati petak " << this->getName() << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
}

void CardTile::getDisplayInfo(std::stringstream& output) const {
    output << "[CardTile] " << name << " (" << code << ")"
           << " category=" << category;}

void CardTile::getTileType(std::stringstream& output) const {
    output << "CARD";
}
