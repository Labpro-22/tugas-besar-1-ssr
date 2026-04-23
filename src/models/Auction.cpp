#include "Auction.hpp"
#include "Player.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <limits>

Auction::Auction(Property* property, vector<Player*> activePlayers, 
    int triggerPlayerIndex, TransactionLogger* logger, int gameTurn)
    : property(property), activePlayers(move(activePlayers)), triggerIndex(triggerPlayerIndex),
    winner(nullptr), finalBid(-1),logger(logger), gameTurn(gameTurn), currentHighBid(-1), 
    currentHighBidder(nullptr), consecutivePasses(0), atLeastOneBid(false) {

    if (!property) {
        throw invalid_argument("Auction: property tidak boleh null.");
    }
    if (this->activePlayers.empty()) {
        throw invalid_argument("Auction: harus ada setidaknya satu pemain aktif.");
    }
    if (triggerIndex < 0 ||
        triggerIndex >= static_cast<int>(this->activePlayers.size())) {
        throw invalid_argument("Auction: triggerIndex di luar batas.");
    }
}

vector<Player*> Auction::buildBidOrder() const {
    vector<Player*> order;
    int n = static_cast<int>(activePlayers.size());
    for (int i = 1; i <= n; ++i) {
        int idx = (triggerIndex + i) % n;
        order.push_back(activePlayers[idx]);
    }
    return order;
}

int Auction::requestAction(Player* player) const {
    printAuctionState();

    cout << "Giliran: " << player->getUsername() << "\n";
    cout << "Uang kamu: M" << player->getMoney() << "\n";

    int minNextBid = (currentHighBid < 0) ? 0 : currentHighBid + 1;

    while (true) {
        cout << "Aksi (PASS / BID <jumlah>): ";
        string line;
        if (!getline(cin, line)) {
            // EOF / stream closed — treat as PASS
            return -1;
        }

        size_t start = line.find_first_not_of(" \t");
        if (start == string::npos) { continue; }
        line = line.substr(start);

        if (line == "PASS" || line == "pass") {
            return -1;
        }

        if (line.rfind("BID ", 0) == 0 || line.rfind("bid ", 0) == 0) {
            string numPart = line.substr(4);
            try {
                int amount = stoi(numPart);

                if (amount < minNextBid) {
                    cout << "Bid harus lebih dari bid sebelumnya (minimal M"
                              << minNextBid << ").\n";
                    continue;
                }
                if (amount > player->getMoney()) {
                    cout << "Uang kamu tidak cukup untuk bid M" << amount
                              << ". Maksimal: M" << player->getMoney() << ".\n";
                    continue;
                }
                return amount;
            } catch (...) {
                cout << "Format tidak valid. Gunakan: BID <angka> atau PASS\n";
            }
            continue;
        }

        cout << "Perintah tidak dikenali. Gunakan: BID <angka> atau PASS\n";
    }
}

void Auction::printAuctionState() const {
    cout << "\n--- Lelang: " << property->getName()
              << " (" << property->getCode() << ") ---\n";
    if (currentHighBidder) {
        cout << "Penawaran tertinggi: M" << currentHighBid
                  << " (" << currentHighBidder->getUsername() << ")\n";
    } else {
        cout << "Belum ada penawaran.\n";
    }
}

void Auction::logBid(Player* bidder, int amount) const {
    if (logger) {
        logger->log(gameTurn, bidder->getUsername(), "LELANG_BID",
                    "Bid M" + to_string(amount) + " untuk " +
                    property->getName() + " (" + property->getCode() + ")");
    }
}

void Auction::logPass(Player* passer) const {
    if (logger) {
        logger->log(gameTurn, passer->getUsername(), "LELANG_PASS",
                    "Pass pada lelang " + property->getName() +
                    " (" + property->getCode() + ")");
    }
}

void Auction::run() {
    int n = static_cast<int>(activePlayers.size());
    // Ending condition: (n - 1) consecutive passes
    int passesNeeded = n - 1;

    cout << "\n+------------------------------------------+\n";
    cout << "  Properti " << property->getName()
              << " (" << property->getCode() << ") akan dilelang!\n";
    cout << "  Urutan lelang dimulai dari pemain setelah "
              << activePlayers[triggerIndex]->getUsername() << ".\n";
    cout << "+------------------------------------------+\n";

    vector<Player*> bidOrder = buildBidOrder();

    bidOrderSize = static_cast<int>(bidOrder.size());
    int cursor = 0;

    while (true) {
        Player* current = bidOrder[cursor % bidOrderSize];
        cursor++;

        // ---- Check one player left can bid ----
        int minNextBid = (currentHighBid < 0) ? 0 : currentHighBid + 1;
        int canBidCount = 0;
        Player* lastCanBid = nullptr;
        for (Player* p : bidOrder) {
            if (p->getMoney() >= minNextBid) {
                canBidCount++;
                lastCanBid = p;
            }
        }

        // Forced bid
        bool mustBid = (!atLeastOneBid &&
                        canBidCount == 1 &&
                        lastCanBid == current);

        if (mustBid) {
            cout << "\n[WAJIB BID] " << current->getUsername()
                      << " adalah satu-satunya peserta yang dapat melakukan bid "
                      << "dan wajib mengajukan penawaran minimal M0.\n";
        }

        int action;
        if (mustBid) {
            // Force minimal bid 
            action = requestForcedBid(current);
        } else {
            action = requestAction(current);
        }

        if (action == -1) {
            // PASS
            consecutivePasses++;
            cout << current->getUsername() << " memilih PASS.\n";
            logPass(current);
        } else {
            // BID
            currentHighBid     = action;
            currentHighBidder  = current;
            consecutivePasses  = 0;
            atLeastOneBid      = true;

            cout << "Penawaran tertinggi: M" << currentHighBid
                      << " (" << currentHighBidder->getUsername() << ")\n";
            logBid(current, action);
        }

        // ---- Ending condition ----
        if (atLeastOneBid && consecutivePasses >= passesNeeded) {
            break;
        }
    }

    // ---- Conclude ----

    if (atLeastOneBid && currentHighBidder != nullptr) {
        winner     = currentHighBidder;
        finalBid = currentHighBid;
        sold       = true;

        cout << "\nLelang selesai!\n";
        cout << "Pemenang: " << result.winner->getUsername() << "\n";
        cout << "Harga akhir: M" << result.winningBid << "\n";
        cout << "Properti " << property->getName()
                  << " (" << property->getCode()
                  << ") kini dimiliki " << result.winner->getUsername() << ".\n";

        // Transfer money
        winner->deductMoney(result.winningBid);
        property->setOwnerID(result.winner->getPlayerIndex());
        property->setStatus(PropertyStatus::OWNED);
        winner->addProperty(property);

        cout << "Uang " << result.winner->getUsername()
                  << ": M" << result.winner->getMoney() << "\n";
    } else {
        winner     = nullptr;
        winningBid = 0;
        sold = false;
        cout << "\nLelang selesai tanpa pemenang. Properti kembali ke Bank.\n";
    }

    logResult(result);
    return result;
}

//  Only accepts valid BID tidak boleh PASS
int Auction::requestForcedBid(Player* player) const {
    printAuctionState();
    cout << "Giliran: " << player->getUsername()
              << " (WAJIB BID — tidak bisa PASS)\n";
    cout << "Uang kamu: M" << player->getMoney() << "\n";

    int minNextBid = (currentHighBid < 0) ? 0 : currentHighBid + 1;

    while (true) {
        cout << "Aksi (BID <jumlah>): ";
        string line;
        if (!getline(cin, line)) {
            cout << "(Auto-bid M0)\n";
            return 0;
        }

        size_t start = line.find_first_not_of(" \t");
        if (start == string::npos) { continue; }
        line = line.substr(start);

        if (line.rfind("BID ", 0) == 0 || line.rfind("bid ", 0) == 0) {
            string numPart = line.substr(4);
            try {
                int amount = stoi(numPart);
                if (amount < minNextBid) {
                    cout << "Bid minimal adalah M" << minNextBid << ".\n";
                    continue;
                }
                if (amount > player->getMoney()) {
                    cout << "Uang tidak cukup. Maksimal: M" << player->getMoney() << ".\n";
                    continue;
                }
                return amount;
            } catch (...) {
                cout << "Format tidak valid. Gunakan: BID <angka>\n";
            }
        } else if (line == "PASS" || line == "pass") {
            cout << "Kamu tidak bisa PASS sekarang. Kamu wajib melakukan BID.\n";
        } else {
            cout << "Perintah tidak dikenali. Gunakan: BID <angka>\n";
        }
    }
}
