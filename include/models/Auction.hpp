#pragma once

#include <string>
#include <vector>

#include "Property.hpp"
#include "TransactionLogger.hpp"

class Player;  // forward declaration

// ============================================================
// AuctionResult
//
// Plain data structure returned after an auction finishes.
// Keeps Auction stateless after run() returns so the Game
// layer can act on the outcome (transfer property, log, etc.)
// ============================================================
struct AuctionResult {
    Player*  winner;       // nullptr if no bids were placed (shouldn't happen per spec)
    int      winningBid;   // final bid amount
    bool     sold;         // true if a winner was determined
};

// ============================================================
// Auction
//
// Responsibilities:
//   - Manage a single auction for one Property.
//   - Cycle through active players starting from the player
//     AFTER the one who triggered the auction.
//   - Accept BID and PASS actions from each player.
//   - Enforce the spec rule: auction ends when
//     (activePlayers.size() - 1) consecutive passes occur
//     after at least one bid has been placed.
//   - If everyone passes with no bid, force the last
//     non-passing player to place a minimum bid of M0.
//   - Return an AuctionResult; let the caller (Game/Bank)
//     actually transfer money and property.
//
// Auction turns do NOT count as game turns (spec §Lelang).
// ============================================================
class Auction {
private:
    Property* property;
    std::vector<Player*> activePlayers;  // only ACTIVE players
    int triggerIndex;   // index in activePlayers of triggering player
    TransactionLogger*     logger;
    intvgameTurn;       // current game turn (for logging)

    // --- Internal state ---
    int     currentHighBid;
    Player* currentHighBidder;
    int     consecutivePasses;
    bool    atLeastOneBid;

    // Ordered list of participants: starts from player AFTER trigger
    std::vector<Player*> buildBidOrder() const;

    // Ask a specific player for their action: returns bid amount or -1 for PASS.
    // For HumanPlayer: reads from stdin.
    // For BotPlayer:   applies simple strategy (bid if can afford, else pass).
    int requestAction(Player* player) const;

    // Display current auction state to stdout.
    void printAuctionState() const;

    // Log a bid event.
    void logBid(Player* bidder, int amount) const;
    void logPass(Player* passer) const;
    void logResult(const AuctionResult& result) const;

    // Like requestAction but PASS is not allowed (spec §5 forced-bid rule).
    int requestForcedBid(Player* player) const;

public:
    // activePlayers must contain only ACTIVE (non-bankrupt) players.
    // triggerPlayerIndex: index inside activePlayers of the player who
    //   triggered the auction (they bid last in the first round).
    Auction(Property* property,
            std::vector<Player*> activePlayers,
            int triggerPlayerIndex,
            TransactionLogger* logger,
            int gameTurn);

    ~Auction() = default;

    // Run the full auction loop. Returns when the ending condition is met.
    // Does NOT transfer money or property — that is the caller's job.
    AuctionResult run();
};
