#include "Tile.hpp"

#include "GameApp.hpp"
#include "AppException.hpp"
#include "GameSession.hpp"
#include "GameApp.hpp"
#include "Player.hpp"
#include "Property.hpp"

#include <vector>
#include <chrono>
#include <thread>

PropertyTile::PropertyTile() : Tile(), property(nullptr) {}

PropertyTile::PropertyTile(int index, const std::string& code, const std::string& name, const std::string& category,
                           Property* property)
    : Tile(index, code, name, category), property(property) {}

void PropertyTile::onLanded(Player* player) {
    if (player == nullptr) {
        throw GameException("PropertyTile", "Player cannot be null.");
    }
    if (property == nullptr) {
        throw GameException("PropertyTile", "Property cannot be null.");
    }


    GameSession *game = GameApp::currentSession;
    int diceNum = game->getLastDiceTotal();
    
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " mendarat di petak " << this->getName() << '\n';

    if (property->getStatus() == PropertyStatus::BANK) {
        if (property->getType() == "RAILROAD" || property->getType() == "UTILITY") {
            std::cout << "Pemain " << player->getUsername() << " mendapatkan properti tipe " << property->getType()  << " secara otomatis\n";
            property->setOwnerID(player->getPlayerIndex());
            property->setStatus(PropertyStatus::OWNED);
            player->addProperty(property);
            return;
        }

        std::stringstream ss;
        property->printCertificate(ss);
        std::cout << ss.rdbuf();

        std::cout << "Pilih aksi untuk petak properti:\n";
        std::cout << "1. Beli properti (M" << property->getPrice() << ")\n";
        std::cout << "2. Lelang properti\n";
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

        rent = street->calculateRent(diceNum, street->buildingCount, street->isMonopolized());
    } 
    else if (property->getType() == "RAILROAD") {
        rent = property->calculateRent(diceNum, owner->getOwnedRailroadCount(), false);
    } 
    else if (property->getType() == "UTILITY") {
        rent = property->calculateRent(diceNum, owner->getOwnedUtilityCount(), false);
    } 
    else {
        throw GameException("PropertyTile", "Unsupported property type: " + property->getType());
    }

    if (player->getShield()) {
        std::cout << "Pemain " << player->getUsername() << " terlindungi oleh ShieldCard! Bebas biaya sewa M" << rent << ".\n";
        return;
    }

    if (!player->canAfford(rent)) {
        game->handleBankruptcy(player, owner, rent);
        return;
    }

    player->deductMoney(rent);
    owner->addMoney(rent);
}

void PropertyTile::onPassed(Player* player) {
    if (player == nullptr) {
        throw GameException("PropertyTile", "Player cannot be null.");
    }
    
    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " melewati petak " << this->getName() << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
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

