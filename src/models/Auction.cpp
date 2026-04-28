#include "Auction.hpp"
#include "Player.hpp"
#include "AppException.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <limits>

// ============================================================
// Constructor
// ============================================================

Auction::Auction(Property* property,
                 std::vector<Player*> activePlayers,
                 int triggerPlayerIndex,
                 TransactionLogger* logger,
                 int gameTurn)
    : property(property),
      activePlayers(std::move(activePlayers)),
      triggerIndex(triggerPlayerIndex),
      logger(logger),
      gameTurn(gameTurn),
      currentHighBid(-1),
      currentHighBidder(nullptr),
      consecutivePasses(0),
      atLeastOneBid(false) {

    if (!property) {
        throw std::invalid_argument("Auction: property tidak boleh null.");
    }
    if (this->activePlayers.empty()) {
        throw std::invalid_argument("Auction: harus ada setidaknya satu pemain aktif.");
    }
    if (triggerIndex < 0 ||
        triggerIndex >= static_cast<int>(this->activePlayers.size())) {
        throw std::invalid_argument("Auction: triggerIndex di luar batas.");
    }
}

std::vector<Player*> Auction::buildBidOrder() const {
    std::vector<Player*> order;
    int n = static_cast<int>(activePlayers.size());
    for (int i = 1; i <= n; ++i) {
        int idx = (triggerIndex + i) % n;
        order.push_back(activePlayers[idx]);
    }
    return order;
}

// ============================================================
// requestAction
//   Returns bid amount (>= 0) or -1 for PASS.
// ============================================================

int Auction::requestAction(Player* player) const {
    // Show current state each time before asking
    printAuctionState();

    std::cout << "Giliran: " << player->getUsername() << "\n";
    std::cout << "Uang kamu: M" << player->getMoney() << "\n";

    int minNextBid = (currentHighBid < 0) ? 0 : currentHighBid + 1;

    while (true) {
        std::cout << "Aksi (PASS / BID <jumlah>): ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            // EOF / stream closed — treat as PASS
            return -1;
        }

        // Trim leading whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) { continue; }
        line = line.substr(start);

        if (line == "PASS" || line == "pass") {
            return -1;
        }

        if (line.rfind("BID ", 0) == 0 || line.rfind("bid ", 0) == 0) {
            std::string numPart = line.substr(4);
            try {
                int amount = std::stoi(numPart);

                if (amount < minNextBid) {
                    std::cout << "Bid harus lebih dari bid sebelumnya (minimal M"
                              << minNextBid << ").\n";
                    continue;
                }
                if (amount > player->getMoney()) {
                    std::cout << "Uang kamu tidak cukup untuk bid M" << amount
                              << ". Maksimal: M" << player->getMoney() << ".\n";
                    continue;
                }
                return amount;
            } catch (AppException &) {
                std::cout << "Format tidak valid. Gunakan: BID <angka> atau PASS\n";
            }
            continue;
        }

        std::cout << "Perintah tidak dikenali. Gunakan: BID <angka> atau PASS\n";
    }
}

// ============================================================
// printAuctionState
// ============================================================

void Auction::printAuctionState() const {
    std::cout << "\n--- Lelang: " << property->getName()
              << " (" << property->getCode() << ") ---\n";
    if (currentHighBidder) {
        std::cout << "Penawaran tertinggi: M" << currentHighBid
                  << " (" << currentHighBidder->getUsername() << ")\n";
    } else {
        std::cout << "Belum ada penawaran.\n";
    }
}

// ============================================================
// Logging helpers
// ============================================================

void Auction::logBid(Player* bidder, int amount) const {
    if (logger) {
        logger->log(gameTurn, bidder->getUsername(), "LELANG_BID",
                    "Bid M" + std::to_string(amount) + " untuk " +
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

void Auction::logResult(const AuctionResult& result) const {
    if (!logger) return;
    if (result.sold && result.winner) {
        logger->log(gameTurn, result.winner->getUsername(), "LELANG_SELESAI",
                    "Menang lelang " + property->getName() +
                    " (" + property->getCode() + ") seharga M" +
                    std::to_string(result.winningBid));
    } else {
        logger->log(gameTurn, "BANK", "LELANG_SELESAI",
                    "Lelang " + property->getName() + " (" + property->getCode() +
                    ") tidak terjual");
    }
}

// ============================================================
// run() – main auction loop
// ============================================================

AuctionResult Auction::run() {
    int n = static_cast<int>(activePlayers.size());
    // Ending condition: (n - 1) consecutive passes after at least one bid.
    int passesNeeded = n - 1;

    std::cout << "\n+------------------------------------------+\n";
    std::cout << "  Properti " << property->getName()
              << " (" << property->getCode() << ") akan dilelang!\n";
    std::cout << "  Urutan lelang dimulai dari pemain setelah "
              << activePlayers[triggerIndex]->getUsername() << ".\n";
    std::cout << "+------------------------------------------+\n";

    std::vector<Player*> bidOrder = buildBidOrder();

    // We loop over bidOrder repeatedly until ending condition.
    // Track which players have been forced out (passed when no alternative).
    // Per spec §5: if everyone passes with no bid, the last non-passing
    // player MUST bid. We track the last player who hasn't passed yet.

    // Active bidders: anyone who hasn't permanently passed
    // (players may pass multiple times in the same auction; each pass
    //  counts toward consecutivePasses).
    // We cycle through bidOrder indefinitely until done.

    int bidOrderSize = static_cast<int>(bidOrder.size());
    int cursor = 0; // index into bidOrder for current turn

    while (true) {
        Player* current = bidOrder[cursor % bidOrderSize];
        cursor++;

        // ---- Check if only one player left can bid ----
        // Count players who could possibly still bid (can afford at least minNextBid)
        int minNextBid = (currentHighBid < 0) ? 0 : currentHighBid + 1;
        int canBidCount = 0;
        Player* lastCanBid = nullptr;
        for (Player* p : bidOrder) {
            if (p->getMoney() >= minNextBid) {
                canBidCount++;
                lastCanBid = p;
            }
        }

        // ---- Forced bid rule (spec §5) ----
        // If no bid yet and this player is the only one who can bid,
        // they MUST bid (skip PASS option).
        bool mustBid = (!atLeastOneBid &&
                        canBidCount == 1 &&
                        lastCanBid == current);

        if (mustBid) {
            std::cout << "\n[WAJIB BID] " << current->getUsername()
                      << " adalah satu-satunya peserta yang dapat melakukan bid "
                      << "dan wajib mengajukan penawaran minimal M0.\n";
        }

        int action;
        if (mustBid) {
            // Force minimal bid of M0
            action = requestForcedBid(current);
        } else {
            action = requestAction(current);
        }

        if (action == -1) {
            // PASS
            consecutivePasses++;
            std::cout << current->getUsername() << " memilih PASS.\n";
            logPass(current);
        } else {
            // BID
            currentHighBid     = action;
            currentHighBidder  = current;
            consecutivePasses  = 0;
            atLeastOneBid      = true;

            std::cout << "Penawaran tertinggi: M" << currentHighBid
                      << " (" << currentHighBidder->getUsername() << ")\n";
            logBid(current, action);
        }

        // ---- Ending condition ----
        if (atLeastOneBid && consecutivePasses >= passesNeeded) {
            break;
        }
    }

    // ---- Conclude ----
    AuctionResult result;

    if (atLeastOneBid && currentHighBidder != nullptr) {
        result.winner     = currentHighBidder;
        result.winningBid = currentHighBid;
        result.sold       = true;

        std::cout << "\nLelang selesai!\n";
        std::cout << "Pemenang: " << result.winner->getUsername() << "\n";
        std::cout << "Harga akhir: M" << result.winningBid << "\n";
        std::cout << "Properti " << property->getName()
                  << " (" << property->getCode()
                  << ") kini dimiliki " << result.winner->getUsername() << ".\n";

        // Transfer: deduct money from winner, assign property
        result.winner->deductMoney(result.winningBid);
        property->setOwnerID(result.winner->getPlayerIndex());
        property->setStatus(PropertyStatus::OWNED);
        result.winner->addProperty(property);

        std::cout << "Uang " << result.winner->getUsername()
                  << ": M" << result.winner->getMoney() << "\n";
    } else {
        // Should not happen per spec, but handle gracefully
        result.winner     = nullptr;
        result.winningBid = 0;
        result.sold       = false;
        std::cout << "\nLelang selesai tanpa pemenang. Properti kembali ke Bank.\n";
    }

    logResult(result);
    return result;
}

// ============================================================
// requestForcedBid
//   Used when a player MUST bid (spec §5).
//   Only accepts a valid BID, no PASS allowed.
// ============================================================

int Auction::requestForcedBid(Player* player) const {
    printAuctionState();
    std::cout << "Giliran: " << player->getUsername()
              << " (WAJIB BID — tidak bisa PASS)\n";
    std::cout << "Uang kamu: M" << player->getMoney() << "\n";

    int minNextBid = (currentHighBid < 0) ? 0 : currentHighBid + 1;

    while (true) {
        std::cout << "Aksi (BID <jumlah>): ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            // Fallback: force M0 bid
            std::cout << "(Auto-bid M0)\n";
            return 0;
        }

        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) { continue; }
        line = line.substr(start);

        if (line.rfind("BID ", 0) == 0 || line.rfind("bid ", 0) == 0) {
            std::string numPart = line.substr(4);
            try {
                int amount = std::stoi(numPart);
                if (amount < minNextBid) {
                    std::cout << "Bid minimal adalah M" << minNextBid << ".\n";
                    continue;
                }
                if (amount > player->getMoney()) {
                    std::cout << "Uang tidak cukup. Maksimal: M" << player->getMoney() << ".\n";
                    continue;
                }
                return amount;
            } catch (AppException &) {
                std::cout << "Format tidak valid. Gunakan: BID <angka>\n";
            }
        } else if (line == "PASS" || line == "pass") {
            std::cout << "Kamu tidak bisa PASS sekarang. Kamu wajib melakukan BID.\n";
        } else {
            std::cout << "Perintah tidak dikenali. Gunakan: BID <angka>\n";
        }
    }
}
