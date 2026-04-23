#include "Bank.hpp"
#include "Player.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>

Bank::Bank(int goSalary,
           int jailFine,
           map<int, int> railroadRentTable,
           map<int, int> utilityMultiplierTable,
           TransactionLogger* logger)
    : goSalary(goSalary),
      jailFine(jailFine),
      railroadRentTable(move(railroadRentTable)),
      utilityMultiplierTable(move(utilityMultiplierTable)),
      logger(logger) {}

void Bank::logTransaction(int turn, const string& username,
                           const string& action,
                           const string& detail) const {
    if (logger) {
        logger->log(turn, username, action, detail);
    }
}

void Bank::buyStreet(Player* buyer, StreetProperty* property, int turn) {
    if (!buyer || !property) {
        throw runtime_error("Bank::buyStreet — null pointer.");
    }
    if (property->getStatus() != PropertyStatus::BANK) {
        throw runtime_error("Properti " + property->getCode() +
                                 " tidak tersedia untuk dibeli (status bukan BANK).");
    }
    if (!buyer->canAfford(property->getPrice())) {
        throw runtime_error("Uang " + buyer->getUsername() +
                                 " tidak cukup untuk membeli " + property->getName() +
                                 ". Harga: M" + to_string(property->getPrice()) +
                                 ", Uang: M" + to_string(buyer->getMoney()) + ".");
    }

    buyer->deductMoney(property->getPrice());
    property->setOwnerID(buyer->getPlayerIndex());
    property->setStatus(PropertyStatus::OWNED);
    buyer->addProperty(property);

    cout << buyer->getUsername() << " membeli " << property->getName()
              << " (" << property->getCode() << ") seharga M"
              << property->getPrice() << ".\n";
    cout << "Uang tersisa: M" << buyer->getMoney() << "\n";

    logTransaction(turn, buyer->getUsername(), "BELI",
                   "Beli " + property->getName() + " (" + property->getCode() +
                   ") seharga M" + to_string(property->getPrice()));
}

void Bank::claimFreeProperty(Player* claimer, Property* property, int turn) {
    if (!claimer || !property) {
        throw runtime_error("Bank::claimFreeProperty — null pointer.");
    }
    if (property->getStatus() != PropertyStatus::BANK) {
        throw runtime_error("Properti " + property->getCode() +
                                 " sudah dimiliki pemain lain.");
    }

    property->setOwnerID(claimer->getPlayerIndex());
    property->setStatus(PropertyStatus::OWNED);
    claimer->addProperty(property);

    cout << "Belum ada yang menginjaknya duluan, "
              << property->getName() << " (" << property->getCode()
              << ") kini menjadi milik " << claimer->getUsername() << "!\n";

    logTransaction(turn, claimer->getUsername(),
                   (property->getType() == "RAILROAD" ? "RAILROAD" : "UTILITY"),
                   property->getName() + " (" + property->getCode() +
                   ") kini milik " + claimer->getUsername() + " (otomatis)");
}

int Bank::sellPropertyToBank(Player* seller, Property* property, int turn) {
    if (!seller || !property) {
        throw runtime_error("Bank::sellPropertyToBank — null pointer.");
    }
    if (property->getOwnerID() != seller->getPlayerIndex()) {
        throw runtime_error("Properti " + property->getCode() +
                                 " bukan milik " + seller->getUsername() + ".");
    }
    if (property->getStatus() == PropertyStatus::MORTGAGED) {
        throw runtime_error("Properti " + property->getCode() +
                                 " sedang digadaikan dan tidak dapat dijual langsung. "
                                 "Tebus terlebih dahulu.");
    }

    int sellValue = property->getSellValue();

    seller->addMoney(sellValue);
    seller->removeProperty(property);
    property->setOwnerID(-1);
    property->setStatus(PropertyStatus::BANK);

    cout << property->getName() << " (" << property->getCode()
              << ") terjual ke Bank. " << seller->getUsername()
              << " menerima M" << sellValue << ".\n";
    cout << "Uang " << seller->getUsername() << ": M" << seller->getMoney() << "\n";

    logTransaction(turn, seller->getUsername(), "JUAL",
                   "Jual " + property->getName() + " (" + property->getCode() +
                   ") ke Bank seharga M" + to_string(sellValue));

    return sellValue;
}

void Bank::mortgageProperty(Player* owner, Property* property, int turn) {
    if (!owner || !property) {
        throw runtime_error("Bank::mortgageProperty — null pointer.");
    }
    if (property->getOwnerID() != owner->getPlayerIndex()) {
        throw runtime_error("Properti " + property->getCode() + " bukan milik " + owner->getUsername() + ".");
    }
    int sellBuilding = 0;
    if(property->type == "STREET"){
        vector<StreetProperty*> propColor = owner->getPropertiesByColor(property->colorGroup);
        for(StreetProperty* prop : propColor){
            if (prop->getBuilding() > 0) sellBuilding = prop->sellBuilding();
        }
    }
    int cash = property->mortgage() + sellBuilding;
    owner->addMoney(cash);

    cout << property->getName() << " (" << property->getCode() << ") berhasil digadaikan.\n";
    cout << owner->getUsername() << " menerima M" << cash << " dari Bank.\n";
    cout << "Uang " << owner->getUsername() << ": M" << owner->getMoney() << "\n";
    cout << "Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.\n";

    logTransaction(turn, owner->getUsername(), "GADAI",
                   "Gadai " + property->getName() + " (" + property->getCode() +
                   "), terima M" + to_string(cash));
}

void Bank::unmortgageProperty(Player* owner, Property* property, int turn) {
    if (!owner || !property) {
        throw runtime_error("Bank::unmortgageProperty — null pointer.");
    }
    if (property->getOwnerID() != owner->getPlayerIndex()) {
        throw runtime_error("Properti " + property->getCode() + " bukan milik " + owner->getUsername() + ".");
    }

    int cost = property->getPrice();
    if (!owner->canAfford(cost)) {
        throw runtime_error("Uang " + owner->getUsername() + " tidak cukup untuk menebus " + property->getName() +
        ". Harga tebus: M" + to_string(cost) + " | Uang: M" + to_string(owner->getMoney()) + ".");
    }

    owner->deductMoney(cost);
    property->unmortgage(); // ubah status -> OWNED

    cout << property->getName() << " (" << property->getCode()
              << ") berhasil ditebus!\n";
    cout << owner->getUsername() << " membayar M" << cost << " ke Bank.\n";
    cout << "Uang " << owner->getUsername() << ": M" << owner->getMoney() << "\n";

    logTransaction(turn, owner->getUsername(), "TEBUS",
                   "Tebus " + property->getName() + " (" + property->getCode() +
                   "), bayar M" + to_string(cost));
}

void Bank::buildOnProperty(Player* owner, StreetProperty* property, int turn) {
    if (!owner || !property) {
        throw runtime_error("Bank::buildOnProperty — null pointer.");
    }
    if (property->getOwnerID() != owner->getPlayerIndex()) {
        throw runtime_error("Properti " + property->getCode() +
                                 " bukan milik " + owner->getUsername() + ".");
    }
    if (!property->canBuild()) {
        throw runtime_error("Properti " + property->getName() +
                                 " sudah mencapai level bangunan maksimum.");
    }

    int cost = property->getBuildCost();
    if (!owner->canAfford(cost)) {
        throw runtime_error("Uang " + owner->getUsername() +
                                 " tidak cukup untuk membangun di " + property->getName() +
                                 ". Biaya: M" + to_string(cost) +
                                 " | Uang: M" + to_string(owner->getMoney()) + ".");
    }

    owner->deductMoney(cost);
    property->build();

    // Determine what was built
    bool isHotel = (property->buildingCount == 5);
    string buildingType = isHotel ? "hotel" : "1 rumah";

    cout << owner->getUsername() << " membangun " << buildingType
              << " di " << property->getName() << " (" << property->getCode()
              << "). Biaya: M" << cost << "\n";
    cout << "Uang tersisa: M" << owner->getMoney() << "\n";

    logTransaction(turn, owner->getUsername(), "BANGUN",
                   "Bangun " + buildingType + " di " + property->getName() +
                   " (" + property->getCode() + "), biaya M" + to_string(cost));
}

void Bank::collectJailFine(Player* payer, int turn) {
    if (!payer) {
        throw runtime_error("Bank::collectJailFine — null pointer.");
    }
    if (!payer->canAfford(jailFine)) {
        throw runtime_error("Uang " + payer->getUsername() +
                                 " tidak cukup untuk membayar denda penjara M" +
                                 to_string(jailFine) + ".");
    }

    payer->deductMoney(jailFine);

    cout << payer->getUsername() << " membayar denda penjara M"
              << jailFine << " ke Bank.\n";
    cout << "Uang " << payer->getUsername() << ": M" << payer->getMoney() << "\n";

    logTransaction(turn, payer->getUsername(), "DENDA_PENJARA",
                   "Bayar denda keluar penjara M" + to_string(jailFine));
}

vector<Property*> Bank::settleBankruptcyToBank(Player* debtor, int turn) {
    if (!debtor) {
        throw runtime_error("Bank::settleBankruptcyToBank — null pointer.");
    }

    cout << "\n" << debtor->getUsername() << " dinyatakan BANGKRUT!\n";
    cout << "Kreditor: Bank\n";
    cout << "Uang sisa M" << debtor->getMoney() << " diserahkan ke Bank.\n";

    logTransaction(turn, debtor->getUsername(), "BANGKRUT",
                   "Bangkrut ke Bank, uang sisa M" + to_string(debtor->getMoney()) +
                   " diserahkan ke Bank");

    // Confiscate remaining cash (disappears from circulation)
    debtor->deductMoney(debtor->getMoney());

    // Reset all properties → BANK, demolish buildings, collect for auction
    vector<Property*> toAuction;
    vector<Property*> ownedProps = debtor->getAllProperties(); // copy

    for (Property* prop : ownedProps) {
        // Demolish buildings if street
        if (prop->getType() == "STREET") {
            StreetProperty* sp = static_cast<StreetProperty*>(prop);
            if (sp->buildingCount > 0) {
                cout << "Bangunan di " << prop->getName() << " dihancurkan.\n";
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

    cout << "Seluruh properti dikembalikan ke status BANK.\n";
    cout << "Bangunan dihancurkan — stok dikembalikan ke Bank.\n";
    cout << "Properti akan dilelang satu per satu.\n";

    return toAuction; // caller (Game) will run Auction for each
}

void Bank::settleBankruptcyToPlayer(Player* debtor, Player* creditor, int turn) {
    if (!debtor || !creditor) {
        throw runtime_error("Bank::settleBankruptcyToPlayer — null pointer.");
    }

    cout << "\n" << debtor->getUsername() << " dinyatakan BANGKRUT!\n";
    cout << "Kreditor: " << creditor->getUsername() << "\n";
    cout << "Pengalihan aset ke " << creditor->getUsername() << ":\n";

    // Transfer remaining cash
    int cash = debtor->getMoney();
    cout << "- Uang tunai sisa: M" << cash << "\n";
    debtor->deductMoney(cash);
    creditor->addMoney(cash);

    // Transfer all properties (status preserved — MORTGAGED stays MORTGAGED)
    vector<Property*> ownedProps = debtor->getAllProperties();
    for (Property* prop : ownedProps) {
        string statusTag = (prop->getStatus() == PropertyStatus::MORTGAGED)
                                ? " MORTGAGED [M]" : " OWNED";
        cout << "- " << prop->getName() << " (" << prop->getCode()
                  << ")" << statusTag << "\n";

        debtor->removeProperty(prop);
        prop->setOwnerID(creditor->getPlayerIndex());
        creditor->addProperty(prop);
    }

    cout << creditor->getUsername() << " menerima semua aset "
              << debtor->getUsername() << ".\n";

    logTransaction(turn, debtor->getUsername(), "BANGKRUT",
                   "Bangkrut ke " + creditor->getUsername() +
                   ", uang M" + to_string(cash) + " + " +
                   to_string(ownedProps.size()) + " properti dialihkan");
}

// Getters
int Bank::getJailFine() const { return jailFine; }