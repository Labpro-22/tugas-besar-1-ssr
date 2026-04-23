#include "Tile.hpp"

#include "AppException.hpp"
#include "Game.hpp"
#include "Player.hpp"

GoToJailTile::GoToJailTile() : Tile() {}

GoToJailTile::GoToJailTile(int index, const std::string& code, const std::string& name, const std::string& category)
    : Tile(index, code, name, category) {}

void GoToJailTile::onLanded(Player* player, Game* game) {
    if (player == nullptr) {
        throw GameException("GoToJailTile", "Player cannot be null.");
    }
    if (game == nullptr) {
        throw GameException("GoToJailTile", "Game cannot be null.");
    }
    if (game->getBoard() == nullptr) {
        throw GameException("GoToJailTile", "Board cannot be null.");
    }

    player->moveTo(game->getBoard(), game->getBoard()->getJailIndex());
    player->setStatus(PlayerStatus::JAILED);
    player->setJailAttempts(0);
}

void GoToJailTile::getDisplayInfo(std::stringstream& output) const {
    output << "[GoToJailTile] " << name << " (" << code << ")"
           << " category=" << category
           << " effect=SEND_TO_JAIL";
}

void GoToJailTile::getTileType(std::stringstream& output) const {
    output << "GO_TO_JAIL";
}
