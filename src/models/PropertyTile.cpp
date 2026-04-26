#include "Tile.hpp"

#include "AppException.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Property.hpp"

#include <vector>

PropertyTile::PropertyTile() : Tile(), property(nullptr) {}

PropertyTile::PropertyTile(int index, const std::string& code, const std::string& name, const std::string& category,
                           Property* property)
    : Tile(index, code, name, category), property(property) {}

void PropertyTile::onLanded(Player* player, Game* game) {
    if (player == nullptr) {
        throw GameException("PropertyTile", "Player cannot be null.");
    }
    if (property == nullptr) {
        throw GameException("PropertyTile", "Property cannot be null.");
    }
    if (game == nullptr) {
        throw GameException("PropertyTile", "Game cannot be null.");
    }
    if (property->getStatus() == PropertyStatus::BANK) {
        if (property->getType() == "RAILROAD" || property->getType() == "UTILITY") {
            property->setOwnerID(player->getPlayerIndex());
            property->setStatus(PropertyStatus::OWNED);
            player->addProperty(property);
            return;
        }

        const int choice = player->chooseInput({1, 2});
        if (choice != 1 && choice != 2) {
            throw PlayerActionException(player, "Invalid property landing choice.");
        }

        if (choice == 1 && player->canAfford(property->getPrice())) {
            player->deductMoney(property->getPrice());
            property->setOwnerID(player->getPlayerIndex());
            property->setStatus(PropertyStatus::OWNED);
            player->addProperty(property);
            return;
        }

        game->handleAuction(property, player);
        return;
    }

    if (property->getOwnerID() == player->getPlayerIndex()) {
        return;
    }

    if (property->getStatus() == PropertyStatus::MORTGAGED) {
        return;
    }

    Player* owner = nullptr;
    for (Player* candidate : game->getPlayers()) {
        if (candidate != nullptr && candidate->getPlayerIndex() == property->getOwnerID()) {
            owner = candidate;
            break;
        }
    }

    if (owner == nullptr) {
        throw GameException("PropertyTile", "Property owner could not be resolved.");
    }

    int rent = 0;
    if (property->getType() == "STREET") {
        StreetProperty* street = dynamic_cast<StreetProperty*>(property);
        if (street == nullptr) {
            throw GameException("PropertyTile", "Street property type did not match StreetProperty instance.");
        }

        const std::vector<Property*> ownedSameColor = owner->getPropertiesByColor(street->colorGroup);
        const std::vector<int> boardSameColor = game->getBoard()->getTilesByColor(street->colorGroup);
        const bool monopolized = !boardSameColor.empty() &&
                                 ownedSameColor.size() == boardSameColor.size();

        rent = street->getRent(0, monopolized ? 1 : 0);
    } else if (property->getType() == "RAILROAD") {
        rent = property->getRent(0, owner->getOwnedRailroadCount());
    } else if (property->getType() == "UTILITY") {
        rent = property->getRent(game->getLastDiceTotal(), owner->getOwnedUtilityCount());
    } else {
        throw GameException("PropertyTile", "Unsupported property type: " + property->getType());
    }

    if (!player->canAfford(rent)) {
        game->handleBankruptcy(player, owner, rent);
        return;
    }

    player->deductMoney(rent);
    owner->addMoney(rent);
}

void PropertyTile::getDisplayInfo(std::stringstream& output) const {
    output << "[PropertyTile] " << name << " (" << code << ")"
           << " category=" << category;

    if (property != nullptr) {
        const char* statusLabel = "UNKNOWN";
        switch (property->getStatus()) {
            case PropertyStatus::BANK:
                statusLabel = "BANK";
                break;
            case PropertyStatus::OWNED:
                statusLabel = "OWNED";
                break;
            case PropertyStatus::MORTGAGED:
                statusLabel = "MORTGAGED";
                break;
        }

        output << " propertyCode=" << property->getCode()
               << " propertyType=" << property->getType()
               << " ownerID=" << property->getOwnerID()
               << " status=" << statusLabel;
    } else {
        output << " property=<null>";
    }
}

void PropertyTile::getTileType(std::stringstream& output) const {
    output << "PROPERTY";
}
