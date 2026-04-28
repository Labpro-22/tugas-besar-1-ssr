#include <iostream>
#include <chrono>
#include <thread>

#include "Tile.hpp"
#include "GameApp.hpp"
#include "AppException.hpp"

FreeParkingTile::FreeParkingTile() : Tile() {}

FreeParkingTile::FreeParkingTile(int index, const std::string& code, const std::string& name,
                                 const std::string& category)
    : Tile(index, code, name, category) {}

void FreeParkingTile::onLanded(Player* player) {
    if (player == nullptr) {
        throw GameException("FreeParkingTile", "Player cannot be null.");
    }

    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " mendarat di petak " << this->getName() << '\n';

    std::cout << "Pemain tidak melakukan apa-apa...\n";
}

void FreeParkingTile::onPassed(Player* player) {
    if (player == nullptr) {
        throw GameException("FreeParkingTile", "Player cannot be null.");
    }
    
    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " melewati petak " << this->getName() << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
}

void FreeParkingTile::getDisplayInfo(std::stringstream& output) const {
    output << "[FreeParkingTile] " << name << " (" << code << ")"
           << " category=" << category
           << " effect=NONE";
}

void FreeParkingTile::getTileType(std::stringstream& output) const {
    output << "FREE_PARKING";
}
