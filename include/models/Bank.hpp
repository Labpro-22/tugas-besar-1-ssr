#pragma once

#include <string>
#include <vector>
#include <map>

#include "Property.hpp"
#include "TransactionLogger.hpp"

class Player;  // forward declaration – avoid circular include with Player.hpp

// ============================================================
// Bank
//
// Responsibilities (Single Responsibility per layer):
//   - Act as the financial counterparty for all property
//     transactions (buy, sell, mortgage, unmortgage).
//   - Pay out GO salary to players.
//   - Collect tax and penalty payments.
//   - Store rent tables for Railroad and Utility so the
//     Game/Tile layer can ask "how much rent is owed?" without
//     hard-coding config values.
//
// The Bank never moves players or advances turns; it only
// handles money and property status changes.
// ============================================================
class Bank {
private:
    // --- Config-driven rent tables ---
    // railroad: key = number of railroads owned, value = rent
    std::map<int, int> railroadRentTable;
    // utility:  key = number of utilities owned, value = multiplier
    std::map<int, int> utilityMultiplierTable;

    int goSalary;   // amount paid when passing/landing on GO
    int jailFine;   // fine to leave jail voluntarily

    TransactionLogger* logger;  // non-owning pointer

    // --- Internal helpers ---
    void logTransaction(int turn, const std::string& username,
                        const std::string& action,
                        const std::string& detail) const;

public:
    // ctor — all config values injected; Bank owns no heap objects
    Bank(int goSalary,
         int jailFine,
         std::map<int, int> railroadRentTable,
         std::map<int, int> utilityMultiplierTable,
         TransactionLogger* logger);

    ~Bank() = default;

    // ----------------------------------------------------------------
    // Property Acquisition
    // ----------------------------------------------------------------

    // Player buys a Street from the bank.
    // Deducts price from player, sets property OWNED + owner.
    // Throws std::runtime_error if player cannot afford or property
    // is not BANK-status.
    void buyStreet(Player* buyer, StreetProperty* property, int turn);

    // Railroad/Utility: first player to land claims it for free.
    void claimFreeProperty(Player* claimer, Property* property, int turn);

    // ----------------------------------------------------------------
    // Property Liquidation  (player → bank)
    // ----------------------------------------------------------------

    // Sell a property back to the bank at getSellValue().
    // Removes property from player, resets to BANK status.
    // Returns the cash the player receives.
    int sellPropertyToBank(Player* seller, Property* property, int turn);

    // ----------------------------------------------------------------
    // Mortgage / Unmortgage
    // ----------------------------------------------------------------

    // Mortgage: player receives mortgageValue, status → MORTGAGED.
    // If the property has buildings they must be sold first; this
    // method assumes the caller (Game) has already done that.
    // Returns cash received by player.
    int mortgageProperty(Player* owner, Property* property, int turn);

    // Unmortgage: player pays full price, status → OWNED.
    // Returns cost deducted from player.
    int unmortgageProperty(Player* owner, Property* property, int turn);

    // ----------------------------------------------------------------
    // Building Management
    // ----------------------------------------------------------------

    // Build one level on a StreetProperty.
    // Deducts build cost from player.
    void buildOnProperty(Player* owner, StreetProperty* property, int turn);

    // Sell one building level back to bank (half price refund).
    // Returns refund amount.
    int sellBuilding(Player* owner, StreetProperty* property, int turn);

    // Sell ALL buildings on a property (used before mortgage / bankruptcy).
    // Returns total refund.
    int sellAllBuildings(Player* owner, StreetProperty* property, int turn);

    // ----------------------------------------------------------------
    // Rent Collection
    // ----------------------------------------------------------------

    // Transfer rent from payer to owner (or bank if owner is bank — 
    // should not normally occur, but guarded).
    void collectRent(Player* payer, Player* owner,
                     Property* property, int rentAmount, int turn);

    // Convenience: look up railroad rent for a given owned-count.
    int getRailroadRent(int ownedCount) const;

    // Convenience: look up utility multiplier for a given owned-count.
    int getUtilityMultiplier(int ownedCount) const;

    // ----------------------------------------------------------------
    // Tax Collection
    // ----------------------------------------------------------------

    // Deduct a flat tax from player and send to bank (lost from circulation).
    void collectFlatTax(Player* payer, int amount,
                        const std::string& taxName, int turn);

    // ----------------------------------------------------------------
    // Salary / Fines
    // ----------------------------------------------------------------

    // Pay GO salary to a player.
    void paySalary(Player* recipient, int turn);

    // Collect jail fine from a player.
    void collectJailFine(Player* payer, int turn);

    // ----------------------------------------------------------------
    // Bankruptcy settlement
    // ----------------------------------------------------------------

    // Bankruptcy TO the bank (tax / card debt):
    //   - Remaining cash taken from player, disappears.
    //   - All properties reset to BANK status (buildings demolished).
    //   - Returns a list of properties to be auctioned by the Game.
    std::vector<Property*> settleBankruptcyToBank(Player* debtor, int turn);

    // Bankruptcy TO another player:
    //   - All cash and properties transferred to creditor.
    //   - Property statuses preserved (MORTGAGED stays MORTGAGED).
    void settleBankruptcyToPlayer(Player* debtor, Player* creditor, int turn);

    // ----------------------------------------------------------------
    // Getters
    // ----------------------------------------------------------------
    int getGoSalary()  const;
    int getJailFine()  const;
};
