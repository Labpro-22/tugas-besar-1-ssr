#include "Tile.hpp"

#include "AppException.hpp"
#include "Card.hpp"
#include "Game.hpp"

CardTile::CardTile() : Tile(), card(nullptr) {}

CardTile::CardTile(int index, const std::string& code, const std::string& name, const std::string& category,
                   Card* card)
    : Tile(index, code, name, category), card(card) {}

void CardTile::onLanded(Player* player, Game* game) {
    if (player == nullptr) {
        throw GameException("CardTile", "Player cannot be null.");
    }
    if (game == nullptr) {
        throw GameException("CardTile", "Game cannot be null.");
    }
    if (card == nullptr) {
        throw GameException("CardTile", "Card cannot be null.");
    }

    card->execute(*player, *game);
}

void CardTile::getDisplayInfo(std::stringstream& output) const {
    output << "[CardTile] " << name << " (" << code << ")"
           << " category=" << category;

    if (card != nullptr) {
        output << " cardId=" << card->id
               << " cardType=" << card->cardType;
    } else {
        output << " card=<null>";
    }
}

void CardTile::getTileType(std::stringstream& output) const {
    output << "CARD";
}
