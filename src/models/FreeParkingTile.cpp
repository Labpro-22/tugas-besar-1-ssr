#include <iostream>

#include "Tile.hpp"
#include "AppException.hpp"

FreeParkingTile::FreeParkingTile() : Tile() {}

FreeParkingTile::FreeParkingTile(int index, const std::string& code, const std::string& name,
                                 const std::string& category)
    : Tile(index, code, name, category) {}

void FreeParkingTile::onLanded(Player* player, GameSession *game) {
    if (player == nullptr) {
        throw GameException("FreeParkingTile", "Player cannot be null.");
    }

    std::cout << "Pemain tidak melakukan apa-apa...\n";
}

void FreeParkingTile::getDisplayInfo(std::stringstream& output) const {
    output << "[FreeParkingTile] " << name << " (" << code << ")"
           << " category=" << category
           << " effect=NONE";
}

void FreeParkingTile::getTileType(std::stringstream& output) const {
    output << "FREE_PARKING";
}
