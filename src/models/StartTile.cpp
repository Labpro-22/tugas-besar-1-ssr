#include "Tile.hpp"

#include "AppException.hpp"
#include "Player.hpp"

StartTile::StartTile() : Tile(), salary(0) {}

StartTile::StartTile(int index, const std::string& code, const std::string& name, const std::string& category, int salary)
    : Tile(index, code, name, category), salary(salary) {}

void StartTile::onPassed(Player* player, Game* game) {
    (void)game;
    if (player == nullptr) {
        throw GameException("StartTile", "Player cannot be null.");
    }
    player->addMoney(salary);
}

void StartTile::onLanded(Player* player, Game* game) {
    onPassed(player, game);
}

void StartTile::getDisplayInfo(std::stringstream& output) const {
    output << "[StartTile] " << name << " (" << code << ")"
           << " category=" << category
           << " salary=" << salary;
}

void StartTile::getTileType(std::stringstream& output) const {
    output << "START";
}
