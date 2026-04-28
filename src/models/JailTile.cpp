#include <iostream>

#include "Tile.hpp"
#include "AppException.hpp"
#include "GameApp.hpp"
#include <chrono>
#include <thread>

JailTile::JailTile() : Tile() {}

JailTile::JailTile(int index, const std::string& code, const std::string& name, const std::string& category)
    : Tile(index, code, name, category) {}

void JailTile::onLanded(Player* player) {
    if (player == nullptr) {
        throw GameException("JailTile", "Player cannot be null.");
    }

    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " mendarat di petak " << this->getName() << '\n';
}

void JailTile::onPassed(Player* player) {
    if (player == nullptr) {
        throw GameException("JailTile", "Player cannot be null.");
    }
    
    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " melewati petak " << this->getName() << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(750));

}

void JailTile::getDisplayInfo(std::stringstream& output) const {
    output << "[JailTile] " << name << " (" << code << ")"
           << " category=" << category
           << " effect=JUST_VISITING";
}

void JailTile::getTileType(std::stringstream& output) const {
    output << "JAIL";
}
