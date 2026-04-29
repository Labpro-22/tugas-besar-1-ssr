#include "Bank.hpp"
#include "Player.hpp"
#include "AppException.hpp"
#include "Auction.hpp"
#include "GameApp.hpp"
#include "Tile.hpp"

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

// ============================================================
// Constructor
// ============================================================

Bank::Bank(int goSalary,
           int jailFine,
           std::map<int, int> railroadRentTable,
           std::map<int, int> utilityMultiplierTable)
    : goSalary(goSalary),
      jailFine(jailFine),
      railroadRentTable(std::move(railroadRentTable)),
      utilityMultiplierTable(std::move(utilityMultiplierTable))
    {}

// ============================================================
// Internal helper
// ============================================================

void Bank::logTransaction(
    int turn, 
    const std::string& username, 
    const std::string& action, 
    const std::string& detail) 
const {
    GameSession *game = GameApp::currentSession;
    if (game->getLogger()) {
        game->getLogger()->log(turn, username, action, detail);
    }
}


// ============================================================
// Property Liquidation
// ============================================================

int Bank::sellPropertyToBank(Player* seller, Property* property, int turn) {
    if (!seller || !property) {
        throw GameException("Bank", "Bank::sellPropertyToBank — null pointer.");
    }
    if (property->getOwnerID() != seller->getPlayerIndex()) {
        throw GameException("Bank", "Properti " + property->getCode() +
                                 " bukan milik " + seller->getUsername() + ".");
    }
    if (property->getStatus() == PropertyStatus::MORTGAGED) {
        throw GameException("Bank", "Properti " + property->getCode() +
                                 " sedang digadaikan dan tidak dapat dijual langsung. "
                                 "Tebus terlebih dahulu.");
    }

    int sellValue = property->getSellValue();

    seller->addMoney(sellValue);
    seller->removeProperty(property);
    property->setOwnerID(-1);
    property->setStatus(PropertyStatus::BANK);

    std::cout << property->getName() << " (" << property->getCode()
              << ") terjual ke Bank. " << seller->getUsername()
              << " menerima M" << sellValue << ".\n";
    std::cout << "Uang " << seller->getUsername() << ": M" << seller->getMoney() << "\n";

    logTransaction(turn, seller->getUsername(), "JUAL",
                   "Jual " + property->getName() + " (" + property->getCode() +
                   ") ke Bank seharga M" + std::to_string(sellValue));

    return sellValue;
}

// ============================================================
// Mortgage / Unmortgage
// ============================================================

int Bank::mortgageProperty(Player* owner, Property* property, int turn) {
    if (!owner || !property) {
        throw GameException("Bank", "Bank::mortgageProperty — null pointer.");
    }
    if (property->getOwnerID() != owner->getPlayerIndex()) {
        throw GameException("Bank", "Properti " + property->getCode() +
                                 " bukan milik " + owner->getUsername() + ".");
    }
    // Delegate status check to Property::mortgage() which throws on wrong status.
    int cash = property->mortgage();
    owner->addMoney(cash);

    std::cout << property->getName() << " (" << property->getCode()
              << ") berhasil digadaikan.\n";
    std::cout << owner->getUsername() << " menerima M" << cash << " dari Bank.\n";
    std::cout << "Uang " << owner->getUsername() << ": M" << owner->getMoney() << "\n";
    std::cout << "Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.\n";

    logTransaction(turn, owner->getUsername(), "GADAI",
                   "Gadai " + property->getName() + " (" + property->getCode() +
                   "), terima M" + std::to_string(cash));

    return cash;
}

int Bank::unmortgageProperty(Player* owner, Property* property, int turn) {
    if (!owner || !property) {
        throw GameException("Bank", "Bank::unmortgageProperty — null pointer.");
    }
    if (property->getOwnerID() != owner->getPlayerIndex()) {
        throw GameException("Bank", "Properti " + property->getCode() +
                                 " bukan milik " + owner->getUsername() + ".");
    }

    int cost = property->getPrice(); // tebus = harga beli penuh (spec)
    if (!owner->canAfford(cost)) {
        throw GameException("Bank", "Uang " + owner->getUsername() +
                                 " tidak cukup untuk menebus " + property->getName() +
                                 ". Harga tebus: M" + std::to_string(cost) +
                                 " | Uang: M" + std::to_string(owner->getMoney()) + ".");
    }

    owner->deductMoney(cost);
    property->unmortgage(); // flips status → OWNED; may throw if not MORTGAGED

    std::cout << property->getName() << " (" << property->getCode()
              << ") berhasil ditebus!\n";
    std::cout << owner->getUsername() << " membayar M" << cost << " ke Bank.\n";
    std::cout << "Uang " << owner->getUsername() << ": M" << owner->getMoney() << "\n";

    logTransaction(turn, owner->getUsername(), "TEBUS",
                   "Tebus " + property->getName() + " (" + property->getCode() +
                   "), bayar M" + std::to_string(cost));

    return cost;
}




// ============================================================
// Bankruptcy Settlement
// ============================================================

std::vector<Property*> Bank::settleBankruptcyToBank(Player* debtor, int turn) {
    if (!debtor) {
        throw GameException("Bank", "Bank::settleBankruptcyToBank — null pointer.");
    }

    std::cout << "\n" << debtor->getUsername() << " dinyatakan BANGKRUT!\n";
    std::cout << "Kreditor: Bank\n";
    std::cout << "Uang sisa M" << debtor->getMoney() << " diserahkan ke Bank.\n";

    logTransaction(turn, debtor->getUsername(), "BANGKRUT",
                   "Bangkrut ke Bank, uang sisa M" + std::to_string(debtor->getMoney()) +
                   " diserahkan ke Bank");

    // Confiscate remaining cash (disappears from circulation)
    debtor->deductMoney(debtor->getMoney());

    // Reset all properties → BANK, demolish buildings, collect for auction
    std::vector<Property*> toAuction;
    std::vector<Property*> ownedProps = debtor->getAllProperties(); // copy

    for (Property* prop : ownedProps) {
        // Demolish buildings if street
        if (prop->getType() == "STREET") {
            StreetProperty* sp = static_cast<StreetProperty*>(prop);
            if (sp->buildingCount > 0) {
                std::cout << "Bangunan di " << prop->getName() << " dihancurkan.\n";
                sp->buildingCount = 0; // direct reset; Bank owns the demolition right
            }
            // Reset festival
            sp->resetFestival();
        }

        prop->setOwnerID(-1);
        prop->setStatus(PropertyStatus::BANK);
        debtor->removeProperty(prop);

        toAuction.push_back(prop);
    }

    std::cout << "Seluruh properti dikembalikan ke status BANK.\n";
    std::cout << "Bangunan dihancurkan dan stok dikembalikan ke Bank.\n";
    std::cout << "Properti akan dilelang satu per satu.\n";

    return toAuction; // caller (Game) will run Auction for each
}

void Bank::settleBankruptcyToPlayer(Player* debtor, Player* creditor, int turn) {
    if (!debtor || !creditor) {
        throw GameException("Bank", "Bank::settleBankruptcyToPlayer — null pointer.");
    }

    std::cout << "\n" << debtor->getUsername() << " dinyatakan BANGKRUT!\n";
    std::cout << "Kreditor: " << creditor->getUsername() << "\n";
    std::cout << "Pengalihan aset ke " << creditor->getUsername() << ":\n";

    // Transfer remaining cash
    int cash = debtor->getMoney();
    std::cout << "- Uang tunai sisa: M" << cash << "\n";
    debtor->deductMoney(cash);
    creditor->addMoney(cash);

    // Transfer all properties (status preserved — MORTGAGED stays MORTGAGED)
    std::vector<Property*> ownedProps = debtor->getAllProperties();
    for (Property* prop : ownedProps) {
        std::string statusTag = (prop->getStatus() == PropertyStatus::MORTGAGED)
                                ? " MORTGAGED [M]" : " OWNED";
        std::cout << "- " << prop->getName() << " (" << prop->getCode()
                  << ")" << statusTag << "\n";

        debtor->removeProperty(prop);
        prop->setOwnerID(creditor->getPlayerIndex());
        creditor->addProperty(prop);
    }

    std::cout << creditor->getUsername() << " menerima semua aset "
              << debtor->getUsername() << ".\n";

    logTransaction(turn, debtor->getUsername(), "BANGKRUT",
                   "Bangkrut ke " + creditor->getUsername() +
                   ", uang M" + std::to_string(cash) + " + " +
                   std::to_string(ownedProps.size()) + " properti dialihkan");
}

// ============================================================
// Getters
// ============================================================

int Bank::getGoSalary() const { return goSalary; }
int Bank::getJailFine() const { return jailFine; }


// MAIN COMMAND 

void Bank::handleAuction(Property *property, Player *player){

    GameSession *game = GameApp::currentSession;

    vector<Player *> activePlayer;
    for(Player *p: game->getPlayers()) if(player->isActive()) activePlayer.push_back(p);

    Auction auction(property, activePlayer, game->getCurrentPlayerIndex(), game->getLogger(), game->getCurrentTurn());
    auction.run();
}


void Bank::handleMortgage(Player* player) {
    GameSession *game = GameApp::currentSession;

    std::vector<Property*> owned; std::vector<int> choices;
    for (size_t i = 0; i < player->getAllProperties().size(); ++i) {
        if (player->getAllProperties()[i]->getStatus() == PropertyStatus::OWNED) {
            owned.push_back(player->getAllProperties()[i]);
            choices.push_back(i + 1);
            std::cout << i + 1 << ". " << owned.back()->getName() << "\n";
        }
    }
    if (choices.empty()) { std::cout << "Tidak ada properti untuk digadaikan.\n"; return; }

    int choice = player->chooseInput(choices);
    Property* p = owned[choice-1];

    if (auto* sp = dynamic_cast<StreetProperty*>(p)) {
        std::vector<int> sameColorIndices = game->getBoard()->getTilesByColor(sp->getColorGroup());
        int totalRefund = 0;
        for (int idx : sameColorIndices) {
            Tile* tile = game->getBoard()->getTile(idx);
            if (auto* pt = dynamic_cast<PropertyTile*>(tile)) {
                if (auto* street = dynamic_cast<StreetProperty*>(pt->property)) {
                    if (street->getBuildingCount() > 0) {
                        totalRefund += street->sellAllBuildings();
                    }
                }
            }
        }
        if (totalRefund > 0) {
            player->addMoney(totalRefund);
            std::cout << "Bangunan di color group yang sama telah dijual ke Bank. Anda menerima M" << totalRefund << ".\n";
        }
    }

    int cash = p->mortgage();
    player->addMoney(cash);
    std::cout << p->getName() << " berhasil digadaikan. Anda menerima M" << cash << ".\n";

    game->log("GADAI", "Pemain " + player->getUsername() + " menggadaikan properti " + p->getCode());
}


void Bank::handleUnmortgage(Player* player) {
    GameSession *game = GameApp::currentSession;

    std::vector<Property*> mortgaged; std::vector<int> choices;
    for (size_t i = 0; i < player->getAllProperties().size(); ++i) {
        if (player->getAllProperties()[i]->getStatus() == PropertyStatus::MORTGAGED) {
            mortgaged.push_back(player->getAllProperties()[i]);
            choices.push_back(i + 1);
            std::cout << i + 1 << ". " << mortgaged.back()->getName() << "\n";
        }
    }

    if (choices.empty()) { std::cout << "Tidak ada properti yang digadaikan.\n"; return; }

    int choice = player->chooseInput(choices);
    Property* p = mortgaged[choice-1];

    int cost = p->getPrice();
    if (player->canAfford(cost)) {
        player->deductMoney(cost);
        p->unmortgage();
        std::cout << p->getName() << " berhasil ditebus seharga M" << cost << ".\n";
        game->log("TEBUS", "Pemain " + player->getUsername() + " menebus properti " + p->getCode());
    } else {
        std::cout << "Uang tidak cukup untuk melakukan tebus (Harga: M" << cost << ")...\n";
    }
}


void Bank::handleBuild(Player* player) {
    GameSession *game = GameApp::currentSession;

    std::vector<StreetProperty*> eligible; 
    std::vector<int> choices;
    for (size_t i = 0; i < player->getAllProperties().size(); ++i) {
        if (auto* sp = dynamic_cast<StreetProperty*>(player->getAllProperties()[i])) {
            if (sp->canBuild()) { eligible.push_back(sp); choices.push_back(i + 1); std::cout << i + 1 << ". " << sp->getName() << "\n"; }
        }
    }

    if (choices.empty()) { std::cout << "Tidak ada properti untuk dibangun.\n"; return; }
    int choice = player->chooseInput(choices);
    StreetProperty* sp = eligible[choice-1];
    
    int cost = sp->getBuildCost();
    if (player->getDiscount() > 0) {
        cost = static_cast<int>(cost * (1.0f - player->getDiscount() / 100.0f));
        std::cout << "Mendapatkan diskon " << player->getDiscount() << "%! Biaya bangun menjadi M" << cost << ".\n";
    }

    if (player->canAfford(cost)) {
        player->deductMoney(cost);
        sp->build();
        game->log("BANGUN", "Pemain " + player->getUsername() + " berhasil membangun lahan " + sp->getCode());
    } 
    else std::cout << "Uang tidak cukup untuk melakukan pembangunan...\n";
}



void Bank::handleBankruptcy(Player* debtor, Player* creditor, int amount) {
    GameSession *game = GameApp::currentSession;
    
    std::string creditorName = creditor ? creditor->getUsername() : "Bank";
    std::string debtorName = debtor->getUsername();

    std::cout << "\nPemain " << debtor->getUsername() << " tidak dapat membayar kewajiban M" << amount << " kepada " << creditorName << "!\n";
    std::cout << "Uang saat ini: M" << debtor->getMoney() << "\n";
    std::cout << "Total kewajiban bayar: M" << amount << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    int maxLiquidation = debtor->getMaxLiquidationValue();
    std::cout << "Estimasi dana maksimum dari likuidasi: M" << maxLiquidation << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    if (maxLiquidation >= amount) {
        std::cout << "Dana likuidasi dapat menutup kewajiban.\n";
        std::cout << "Kamu wajib melikuidasi aset untuk membayar.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        while (debtor->getMoney() < amount) {
            std::cout << "\n========== Panel Likuidasi ==========\n";
            std::cout << "Uang kamu saat ini: M" << debtor->getMoney() << " | Kewajiban: M" << amount << "\n";
            
            std::vector<LiquidationAction> availableActions;

            std::cout << "[Jual ke Bank]\n";
            for (auto p : debtor->getAllProperties()) {
                if (p->getStatus() == PropertyStatus::OWNED) {
                    availableActions.push_back({1, p, p->getSellValue()});
                    std::cout << availableActions.size() << ". " << p->getName() << " (" << p->getCode() << ") Harga Jual: M" << p->getSellValue();
                    if (auto* sp = dynamic_cast<StreetProperty*>(p)) {
                        if (sp->buildingCount > 0) std::cout << " (termasuk bangunan)";
                    }
                    std::cout << "\n";
                }
            }

            int mortgageStart = availableActions.size();
            std::cout << "[Gadaikan]\n";
            for (auto p : debtor->getAllProperties()) {
                if (p->getStatus() == PropertyStatus::OWNED) {
                    availableActions.push_back({2, p, p->getMortgageValue()});
                    std::cout << availableActions.size() << ". " << p->getName() << " (" << p->getCode() << ") Nilai Gadai: M" << p->getMortgageValue() << "\n";
                }
            }

            if (availableActions.empty()) break;

            std::cout << "Pilih aksi: ";
            std::vector<int> choices;
            for(size_t i=1; i<=availableActions.size(); ++i) choices.push_back(i);
            int choice = debtor->chooseInput(choices);
            
            LiquidationAction action = availableActions[choice - 1];
            if (action.type == 1) {
                sellPropertyToBank(debtor, action.prop, game->getCurrentTurn());
                game->log("LIKUIDASI", "Pemain " + debtorName + " melikuidasi " + action.prop->getCode() + ", dengan menjual ke Bank");
            } 
            else {
                mortgageProperty(debtor, action.prop, game->getCurrentTurn());
                game->log("LIKUIDASI", "Pemain " + debtorName + " melikuidasi " + action.prop->getCode() + ", dengan menggadai ke Bank");
            }
        }

        if (debtor->getMoney() >= amount) {
            std::cout << "Kewajiban M" << amount << " terpenuhi. Membayar ke " << creditorName << "...\n";
            int prevDebtorMoney = debtor->getMoney();
            debtor->deductMoney(amount);
            std::cout << "Uang kamu : M" << prevDebtorMoney << " M" << debtor->getMoney() << "\n";
            if (creditor) {
                int prevCreditorMoney = creditor->getMoney();
                creditor->addMoney(amount);
                std::cout << "Uang " << creditorName << ": M" << prevCreditorMoney << " M" << creditor->getMoney() << "\n";
            }
            return;
        }
    }

    debtor->setStatus(PlayerStatus::BANKRUPT);

    if (creditor) {
        settleBankruptcyToPlayer(debtor, creditor, game->getCurrentTurn());
    } 
    else 
    {
        std::vector<Property*> toAuction = settleBankruptcyToBank(debtor, game->getCurrentTurn());
        for (Property* p : toAuction) {
            std::cout << "\n Lelang: " << p->getName() << " (" << p->getCode() << ") ...\n";
            std::vector<Player*> activeOthers;
            for(Player* op : game->getPlayers()) if(op != debtor && op->isActive()) activeOthers.push_back(op);
            if (!activeOthers.empty()) {
                Auction auction(p, activeOthers, game->getCurrentPlayerIndex(), game->getLogger(), game->getCurrentTurn());
                auction.run();
            }
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    std::cout << "\nPemain " << debtor->getUsername() << " telah keluar dari permainan.\n";
    int activeCount = 0;
    for(Player* p : game->getPlayers()) if(p->isActive()) activeCount++;

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    std::cout << "Permainan berlanjut dengan " << activeCount << " pemain tersisa.\n";
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    game->log("BANGKRUT", "Pemain " + debtorName + " bangkrut kepada " + creditorName + " (Kewajiban: M" + std::to_string(amount) + ")");
}