#pragma once

#include <string>
#include <vector>
#include <map>

#include "Player.hpp"
#include "Property.hpp"
#include "TransactionLogger.hpp"
using namespace std;

class Bank {
private:
    class LiquidationAction {
    public:
        int type; 
        Property* prop;
        int value;
    };

    int goSalary;
    int jailFine;
    std::map<int, int> utilityMultiplierTable;
    std::map<int, int> railroadRentTable;

    void logTransaction(int turn, const string& username,
                        const string& action,
                        const string& detail) const;

public:
    Bank(int goSalary,
           int jailFine,
           std::map<int, int> railroadRentTable,
           std::map<int, int> utilityMultiplierTable);

    ~Bank() = default;
    void buyStreet(Player* buyer, StreetProperty* property, int turn);

    void claimFreeProperty(Player* claimer, Property* property, int turn);

    // Property Liquidation  (player -> bank)
    int sellPropertyToBank(Player* seller, Property* property, int turn);

    int mortgageProperty(Player* owner, Property* property, int turn);

    int unmortgageProperty(Player* owner, Property* property, int turn);

    int sellBuilding(Player* owner, StreetProperty* property, int turn);

    int sellAllBuildings(Player* owner, StreetProperty* property, int turn);

    void buildOnProperty(Player* owner, StreetProperty* property, int turn);

    int getRailroadRent(int ownedCount) const;

    int getUtilityMultiplier(int ownedCount) const;

    void collectJailFine(Player* payer, int turn);

    void collectRent(Player* payer, Player* owner, Property* property, int rentAmount, int turn);

    void collectFlatTax(Player* payer, int amount, const std::string& taxName, int turn);

    vector<Property*> settleBankruptcyToBank(Player* debtor, int turn);

    void paySalary(Player* recipient, int turn);

    void settleBankruptcyToPlayer(Player* debtor, Player* creditor, int turn);

    // ----------------------------------------------------------------
    // Getters
    // ----------------------------------------------------------------
    int getGoSalary()  const;
    int getJailFine()  const;


    // MAIN COMMAND
    void handleBankruptcy(Player* debtor, Player* creditor, int amount);
    void handleBuild(Player* player);
    void handleUnmortgage(Player* player);
    void handleMortgage(Player* player);
    void handleAuction(Property *property, Player *player);
};
