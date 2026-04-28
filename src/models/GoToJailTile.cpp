#include <iostream>
#include <chrono>
#include <thread>

#include "Tile.hpp"
#include "GameApp.hpp"
#include "AppException.hpp"
#include "GameSession.hpp"
#include "GameApp.hpp"
#include "Player.hpp"


GoToJailTile::GoToJailTile() : Tile() {}

GoToJailTile::GoToJailTile(int index, const std::string& code, const std::string& name, const std::string& category)
    : Tile(index, code, name, category) {}

void GoToJailTile::onLanded(Player* player) {
    if (player == nullptr) {
        throw GameException("GoToJailTile", "Player cannot be null.");
    }

    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " mendarat di petak " << this->getName() << '\n';
    std::cout << "Pemain masuk penjara dan tidak akan bisa beraksi pada giliran selanjutnya!\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(750));

    player->moveTo(game->getBoard(), game->getBoard()->getJailIndex());
    game->getBoard()->printBoard();
    player->setStatus(PlayerStatus::JAILED);
    player->setJailAttempts(0);
}

void GoToJailTile::onPassed(Player* player) {
    if (player == nullptr) {
        throw GameException("GoToJailTile", "Player cannot be null.");
    }
    
    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " melewati petak " << this->getName() << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
}

void GoToJailTile::getDisplayInfo(std::stringstream& output) const {
    output << "[GoToJailTile] " << name << " (" << code << ")"
           << " category=" << category
           << " effect=SEND_TO_JAIL";
}

void GoToJailTile::getTileType(std::stringstream& output) const {
    output << "GO_TO_JAIL";
}
