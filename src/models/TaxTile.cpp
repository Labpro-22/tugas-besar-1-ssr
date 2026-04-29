#include "Tile.hpp"

#include "GameApp.hpp"
#include "AppException.hpp"
#include "GameSession.hpp"
#include "Player.hpp"
#include "GameApp.hpp"
#include <iostream>
#include <chrono>
#include <thread>

TaxTile::TaxTile()
    : Tile(), taxType(TAX_PPH), flatAmount(0), percentage(0.0F) {}

TaxTile::TaxTile(int index, const std::string& code, const std::string& name, const std::string& category,
                 TaxType taxType, int flatAmount, float percentage)
    : Tile(index, code, name, category), taxType(taxType), flatAmount(flatAmount), percentage(percentage) {}

void TaxTile::onLanded(Player* player) {
    if (player == nullptr) {
        throw GameException("TaxTile", "Player cannot be null.");
    }
    if (flatAmount < 0) {
        throw GameException("TaxTile", "Flat tax amount cannot be negative.");
    }
    if (percentage < 0.0F) {
        throw GameException("TaxTile", "Tax percentage cannot be negative.");
    }


    GameSession *game = GameApp::currentSession;

    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " mendarat di petak " << this->getName() << '\n';

    if (player->getShield()) {
        std::cout << "Pemain " << player->getUsername() << " terlindungi oleh ShieldCard! Bebas biaya pajak.\n";
        return;
    }

    if (taxType == TAX_PBM) {
        std::cout << "Pemain " << player->getUsername() << " harus membayar pajak " << (taxType == TAX_PPH ? "PPH" : "PBM") << " sebesar " << std::to_string(flatAmount) << '\n';
        if (!player->canAfford(flatAmount)) {
            std::cout << "Pemain tidak dapat membayar pajak tersebut...\n";
            if (game == nullptr) {
                throw GameException("TaxTile", "Game cannot be null when PBM triggers bankruptcy flow.");
            }
            game->getBank()->handleBankruptcy(player, nullptr, flatAmount);
            return;
        }
        player->deductMoney(flatAmount);
        std::cout << "Pemain berhasil membayar pajak!\n";
        game->log("TAX", "Pemain " + player->getUsername() + " membayar pajak " + (taxType == TAX_PPH ? "PPH" : "PBM") + " sebesar " + std::to_string(flatAmount));
        return;
    }

    std::cout << "Pilih metode pembayaran pajak:\n";
    std::cout << "1. Bayar nominal tetap (M" << flatAmount << ")\n";
    std::cout << "2. Bayar persentase kekayaan (" << percentage << "%)\n";
    const int choice = player->chooseInput({1, 2});
    if (choice != 1 && choice != 2) {
        throw PlayerActionException(player, "Invalid tax payment choice.");
    }

    if (choice == 1) {
        std::cout << "Pemain " + player->getUsername() + " harus membayar pajak " << (taxType == TAX_PPH ? "PPH" : "PBM") << " sebesar " << std::to_string(flatAmount) << '\n';
        
        if (!player->canAfford(flatAmount)) {
            std::cout << "Pemain tidak dapat membayar pajak tersebut...\n";
            if (game == nullptr) {
                throw GameException("TaxTile", "Game cannot be null when flat PPH triggers bankruptcy flow.");
            }
            game->getBank()->handleBankruptcy(player, nullptr, flatAmount);
            return;
        }
        player->deductMoney(flatAmount);
        std::cout << "Pemain berhasil membayar pajak!\n";
        return;
    }

    const int totalWealth = player->getTotalWealth();
    const int taxAmount = static_cast<int>((percentage / 100 * static_cast<float>(totalWealth)) + 0.5F);

    std::cout << "Pemain " + player->getUsername() + " harus membayar pajak " << (taxType == TAX_PPH ? "PPH" : "PBM") << " sebesar " << std::to_string(taxAmount) << '\n';

    if (!player->canAfford(taxAmount)) {
        std::cout << "Pemain tidak dapat membayar pajak tersebut...\n";

        if (game == nullptr) {
            throw GameException("TaxTile", "Game cannot be null when percentage PPH triggers bankruptcy flow.");
        }
        game->getBank()->handleBankruptcy(player, nullptr, taxAmount);
        return;
    }

    game->log("TAX", "Pemain " + player->getUsername() + " membayar pajak " + (taxType == TAX_PPH ? "PPH" : "PBM") + " sebesar " + std::to_string(taxAmount));
    player->deductMoney(taxAmount);
    std::cout << "Pemain berhasil membayar pajak!\n";

}


void TaxTile::onPassed(Player* player) {
    if (player == nullptr) {
        throw GameException("TaxTile", "Player cannot be null.");
    }
    
    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " melewati petak " << this->getName() + "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
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
