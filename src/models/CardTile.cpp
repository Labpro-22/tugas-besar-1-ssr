#include "Tile.hpp"

#include "AppException.hpp"
#include "Card.hpp"
#include "GameSession.hpp"
#include <iostream>

CardTile::CardTile() : Tile(), card(nullptr) {}

CardTile::CardTile(int index, const std::string& code, const std::string& name, const std::string& category,
                   Card* card)
    : Tile(index, code, name, category), card(card) {}

void CardTile::onLanded(Player* player, GameSession* game) {
    if (player == nullptr) {
        throw GameException("CardTile", "Player cannot be null.");
    }
    if (game == nullptr) {
        throw GameException("CardTile", "Game cannot be null.");
    }
    
    std::cout << "Kamu mendarat di Petak " << name << "!\n";
    game->drawCard(player);
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
