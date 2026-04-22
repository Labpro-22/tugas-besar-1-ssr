#include "Bank.hpp"
#include "Player.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>

// ============================================================
// Constructor
// ============================================================

Bank::Bank(int goSalary,
           int jailFine,
           std::map<int, int> railroadRentTable,
           std::map<int, int> utilityMultiplierTable,
           TransactionLogger* logger)
    : goSalary(goSalary),
      jailFine(jailFine),
      railroadRentTable(std::move(railroadRentTable)),
      utilityMultiplierTable(std::move(utilityMultiplierTable)),
      logger(logger) {}

// ============================================================
// Internal helper
// ============================================================

void Bank::logTransaction(int turn, const std::string& username,
                           const std::string& action,
                           const std::string& detail) const {
    if (logger) {
        logger->log(turn, username, action, detail);
    }
}

// ============================================================
// Property Acquisition
// ============================================================

void Bank::buyStreet(Player* buyer, StreetProperty* property, int turn) {
    if (!buyer || !property) {
        throw std::runtime_error("Bank::buyStreet — null pointer.");
    }
    if (property->getStatus() != PropertyStatus::BANK) {
        throw std::runtime_error("Properti " + property->getCode() +
                                 " tidak tersedia untuk dibeli (status bukan BANK).");
    }
    if (!buyer->canAfford(property->getPrice())) {
        throw std::runtime_error("Uang " + buyer->getUsername() +
                                 " tidak cukup untuk membeli " + property->getName() +
                                 ". Harga: M" + std::to_string(property->getPrice()) +
                                 ", Uang: M" + std::to_string(buyer->getMoney()) + ".");
    }

    buyer->deductMoney(property->getPrice());
    property->setOwnerID(buyer->getPlayerIndex());
    property->setStatus(PropertyStatus::OWNED);
    buyer->addProperty(property);

    std::cout << buyer->getUsername() << " membeli " << property->getName()
              << " (" << property->getCode() << ") seharga M"
              << property->getPrice() << ".\n";
    std::cout << "Uang tersisa: M" << buyer->getMoney() << "\n";

    logTransaction(turn, buyer->getUsername(), "BELI",
                   "Beli " + property->getName() + " (" + property->getCode() +
                   ") seharga M" + std::to_string(property->getPrice()));
}

void Bank::claimFreeProperty(Player* claimer, Property* property, int turn) {
    if (!claimer || !property) {
        throw std::runtime_error("Bank::claimFreeProperty — null pointer.");
    }
    if (property->getStatus() != PropertyStatus::BANK) {
        throw std::runtime_error("Properti " + property->getCode() +
                                 " sudah dimiliki pemain lain.");
    }

    property->setOwnerID(claimer->getPlayerIndex());
    property->setStatus(PropertyStatus::OWNED);
    claimer->addProperty(property);

    std::cout << "Belum ada yang menginjaknya duluan, "
              << property->getName() << " (" << property->getCode()
              << ") kini menjadi milik " << claimer->getUsername() << "!\n";

    logTransaction(turn, claimer->getUsername(),
                   (property->getType() == "RAILROAD" ? "RAILROAD" : "UTILITY"),
                   property->getName() + " (" + property->getCode() +
                   ") kini milik " + claimer->getUsername() + " (otomatis)");
}

// ============================================================
// Property Liquidation
// ============================================================

int Bank::sellPropertyToBank(Player* seller, Property* property, int turn) {
    if (!seller || !property) {
        throw std::runtime_error("Bank::sellPropertyToBank — null pointer.");
    }
    if (property->getOwnerID() != seller->getPlayerIndex()) {
        throw std::runtime_error("Properti " + property->getCode() +
                                 " bukan milik " + seller->getUsername() + ".");
    }
    if (property->getStatus() == PropertyStatus::MORTGAGED) {
        throw std::runtime_error("Properti " + property->getCode() +
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
        throw std::runtime_error("Bank::mortgageProperty — null pointer.");
    }
    if (property->getOwnerID() != owner->getPlayerIndex()) {
        throw std::runtime_error("Properti " + property->getCode() +
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
        throw std::runtime_error("Bank::unmortgageProperty — null pointer.");
    }
    if (property->getOwnerID() != owner->getPlayerIndex()) {
        throw std::runtime_error("Properti " + property->getCode() +
                                 " bukan milik " + owner->getUsername() + ".");
    }

    int cost = property->getPrice(); // tebus = harga beli penuh (spec)
    if (!owner->canAfford(cost)) {
        throw std::runtime_error("Uang " + owner->getUsername() +
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
// Building Management
// ============================================================

void Bank::buildOnProperty(Player* owner, StreetProperty* property, int turn) {
    if (!owner || !property) {
        throw std::runtime_error("Bank::buildOnProperty — null pointer.");
    }
    if (property->getOwnerID() != owner->getPlayerIndex()) {
        throw std::runtime_error("Properti " + property->getCode() +
                                 " bukan milik " + owner->getUsername() + ".");
    }
    if (!property->canBuild()) {
        throw std::runtime_error("Properti " + property->getName() +
                                 " sudah mencapai level bangunan maksimum.");
    }

    int cost = property->getBuildCost();
    if (!owner->canAfford(cost)) {
        throw std::runtime_error("Uang " + owner->getUsername() +
                                 " tidak cukup untuk membangun di " + property->getName() +
                                 ". Biaya: M" + std::to_string(cost) +
                                 " | Uang: M" + std::to_string(owner->getMoney()) + ".");
    }

    owner->deductMoney(cost);
    property->build();

    // Determine what was built
    bool isHotel = (property->buildingCount == 5);
    std::string buildingType = isHotel ? "hotel" : "1 rumah";

    std::cout << owner->getUsername() << " membangun " << buildingType
              << " di " << property->getName() << " (" << property->getCode()
              << "). Biaya: M" << cost << "\n";
    std::cout << "Uang tersisa: M" << owner->getMoney() << "\n";

    logTransaction(turn, owner->getUsername(), "BANGUN",
                   "Bangun " + buildingType + " di " + property->getName() +
                   " (" + property->getCode() + "), biaya M" + std::to_string(cost));
}

int Bank::sellBuilding(Player* owner, StreetProperty* property, int turn) {
    if (!owner || !property) {
        throw std::runtime_error("Bank::sellBuilding — null pointer.");
    }

    bool wasHotel = (property->buildingCount == 5);
    int refund = property->sellBuilding(); // throws if no buildings
    owner->addMoney(refund);

    std::string what = wasHotel ? "hotel" : "1 rumah";
    std::cout << "Bangunan " << property->getName() << " (" << what
              << ") terjual. " << owner->getUsername()
              << " menerima M" << refund << ".\n";

    logTransaction(turn, owner->getUsername(), "JUAL_BANGUNAN",
                   "Jual " + what + " di " + property->getName() +
                   " (" + property->getCode() + "), terima M" + std::to_string(refund));

    return refund;
}

int Bank::sellAllBuildings(Player* owner, StreetProperty* property, int turn) {
    if (!owner || !property) {
        throw std::runtime_error("Bank::sellAllBuildings — null pointer.");
    }

    int total = 0;
    while (property->buildingCount > 0) {
        total += sellBuilding(owner, property, turn);
    }
    return total;
}

// ============================================================
// Rent Collection
// ============================================================

void Bank::collectRent(Player* payer, Player* owner,
                        Property* property, int rentAmount, int turn) {
    if (!payer || !owner || !property) {
        throw std::runtime_error("Bank::collectRent — null pointer.");
    }
    if (property->getStatus() == PropertyStatus::MORTGAGED) {
        std::cout << "Properti " << property->getName() << " (" << property->getCode()
                  << ") sedang digadaikan [M]. Tidak ada sewa yang dikenakan.\n";
        return;
    }
    if (!payer->canAfford(rentAmount)) {
        // Let the caller handle bankruptcy; just throw to signal it.
        throw std::runtime_error("BANKRUPT:" + payer->getUsername() +
                                 " tidak mampu membayar sewa M" +
                                 std::to_string(rentAmount) + " kepada " +
                                 owner->getUsername() + ".");
    }

    payer->deductMoney(rentAmount);
    owner->addMoney(rentAmount);

    std::cout << payer->getUsername() << " membayar sewa M" << rentAmount
              << " kepada " << owner->getUsername()
              << " (" << property->getName() << ").\n";
    std::cout << "Uang " << payer->getUsername() << ": M" << payer->getMoney() << "\n";
    std::cout << "Uang " << owner->getUsername() << ": M" << owner->getMoney() << "\n";

    logTransaction(turn, payer->getUsername(), "SEWA",
                   "Bayar M" + std::to_string(rentAmount) + " ke " +
                   owner->getUsername() + " (" + property->getName() + ")");
}

int Bank::getRailroadRent(int ownedCount) const {
    auto it = railroadRentTable.find(ownedCount);
    if (it == railroadRentTable.end()) {
        return 0;
    }
    return it->second;
}

int Bank::getUtilityMultiplier(int ownedCount) const {
    auto it = utilityMultiplierTable.find(ownedCount);
    if (it == utilityMultiplierTable.end()) {
        return 0;
    }
    return it->second;
}

// ============================================================
// Tax Collection
// ============================================================

void Bank::collectFlatTax(Player* payer, int amount,
                           const std::string& taxName, int turn) {
    if (!payer) {
        throw std::runtime_error("Bank::collectFlatTax — null pointer.");
    }
    if (!payer->canAfford(amount)) {
        throw std::runtime_error("BANKRUPT:" + payer->getUsername() +
                                 " tidak mampu membayar " + taxName +
                                 " M" + std::to_string(amount) + ".");
    }

    payer->deductMoney(amount);

    std::cout << payer->getUsername() << " membayar " << taxName
              << " M" << amount << " ke Bank.\n";
    std::cout << "Uang " << payer->getUsername() << ": M" << payer->getMoney() << "\n";

    logTransaction(turn, payer->getUsername(), "PAJAK",
                   taxName + " M" + std::to_string(amount) + " dibayar ke Bank");
}

// ============================================================
// Salary / Fines
// ============================================================

void Bank::paySalary(Player* recipient, int turn) {
    if (!recipient) {
        throw std::runtime_error("Bank::paySalary — null pointer.");
    }

    recipient->addMoney(goSalary);

    std::cout << recipient->getUsername() << " menerima gaji M"
              << goSalary << " dari Bank (melewati/berhenti di GO).\n";
    std::cout << "Uang " << recipient->getUsername() << ": M"
              << recipient->getMoney() << "\n";

    logTransaction(turn, recipient->getUsername(), "GAJI",
                   "Terima gaji M" + std::to_string(goSalary) + " dari Bank (GO)");
}

void Bank::collectJailFine(Player* payer, int turn) {
    if (!payer) {
        throw std::runtime_error("Bank::collectJailFine — null pointer.");
    }
    if (!payer->canAfford(jailFine)) {
        throw std::runtime_error("Uang " + payer->getUsername() +
                                 " tidak cukup untuk membayar denda penjara M" +
                                 std::to_string(jailFine) + ".");
    }

    payer->deductMoney(jailFine);

    std::cout << payer->getUsername() << " membayar denda penjara M"
              << jailFine << " ke Bank.\n";
    std::cout << "Uang " << payer->getUsername() << ": M" << payer->getMoney() << "\n";

    logTransaction(turn, payer->getUsername(), "DENDA_PENJARA",
                   "Bayar denda keluar penjara M" + std::to_string(jailFine));
}

// ============================================================
// Bankruptcy Settlement
// ============================================================

std::vector<Property*> Bank::settleBankruptcyToBank(Player* debtor, int turn) {
    if (!debtor) {
        throw std::runtime_error("Bank::settleBankruptcyToBank — null pointer.");
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
    std::cout << "Bangunan dihancurkan — stok dikembalikan ke Bank.\n";
    std::cout << "Properti akan dilelang satu per satu.\n";

    return toAuction; // caller (Game) will run Auction for each
}

void Bank::settleBankruptcyToPlayer(Player* debtor, Player* creditor, int turn) {
    if (!debtor || !creditor) {
        throw std::runtime_error("Bank::settleBankruptcyToPlayer — null pointer.");
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
