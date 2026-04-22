#include "Tile.hpp"

#include "AppException.hpp"
#include "Game.hpp"
#include "Player.hpp"

TaxTile::TaxTile()
    : Tile(), taxType(TAX_PPH), flatAmount(0), percentage(0.0F) {}

TaxTile::TaxTile(int index, const std::string& code, const std::string& name, const std::string& category,
                 TaxType taxType, int flatAmount, float percentage)
    : Tile(index, code, name, category), taxType(taxType), flatAmount(flatAmount), percentage(percentage) {}

void TaxTile::onLanded(Player* player, Game* game) {
    if (player == nullptr) {
        throw GameException("TaxTile", "Player cannot be null.");
    }
    if (flatAmount < 0) {
        throw GameException("TaxTile", "Flat tax amount cannot be negative.");
    }
    if (percentage < 0.0F) {
        throw GameException("TaxTile", "Tax percentage cannot be negative.");
    }

    if (taxType == TAX_PBM) {
        if (!player->canAfford(flatAmount)) {
            if (game == nullptr) {
                throw GameException("TaxTile", "Game cannot be null when PBM triggers bankruptcy flow.");
            }
            game->handleBankruptcy(player, nullptr, flatAmount);
            return;
        }
        player->deductMoney(flatAmount);
        return;
    }

    const int choice = player->chooseInput({1, 2});
    if (choice != 1 && choice != 2) {
        throw PlayerActionException(player, "Invalid tax payment choice.");
    }

    if (choice == 1) {
        if (!player->canAfford(flatAmount)) {
            if (game == nullptr) {
                throw GameException("TaxTile", "Game cannot be null when flat PPH triggers bankruptcy flow.");
            }
            game->handleBankruptcy(player, nullptr, flatAmount);
            return;
        }
        player->deductMoney(flatAmount);
        return;
    }

    const int totalWealth = player->getTotalWealth();
    const int taxAmount = static_cast<int>((percentage * static_cast<float>(totalWealth)) + 0.5F);

    if (!player->canAfford(taxAmount)) {
        if (game == nullptr) {
            throw GameException("TaxTile", "Game cannot be null when percentage PPH triggers bankruptcy flow.");
        }
        game->handleBankruptcy(player, nullptr, taxAmount);
        return;
    }
    player->deductMoney(taxAmount);
}

void TaxTile::getDisplayInfo(std::stringstream& output) const {
    output << "[TaxTile] " << name << " (" << code << ")"
           << " category=" << category
           << " taxType=" << (taxType == TAX_PPH ? "PPH" : "PBM")
           << " flatAmount=" << flatAmount
           << " percentage=" << percentage;
}

void TaxTile::getTileType(std::stringstream& output) const {
    output << "TAX";
}
