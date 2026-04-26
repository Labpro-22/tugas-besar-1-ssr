#include "Tile.hpp"

#include "AppException.hpp"

JailTile::JailTile() : Tile() {}

JailTile::JailTile(int index, const std::string& code, const std::string& name, const std::string& category)
    : Tile(index, code, name, category) {}

void JailTile::onLanded(Player* player, GameSession* game) {
    if (player == nullptr) {
        throw GameException("JailTile", "Player cannot be null.");
    }
    (void)game;
}

void JailTile::getDisplayInfo(std::stringstream& output) const {
    output << "[JailTile] " << name << " (" << code << ")"
           << " category=" << category
           << " effect=JUST_VISITING";
}

void JailTile::getTileType(std::stringstream& output) const {
    output << "JAIL";
}
