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
    int jailFine;
    TransactionLogger* logger;

    void logTransaction(int turn, const string& username,
                        const string& action,
                        const string& detail) const;

public:
    Bank(int jailFine,TransactionLogger* logger);
    ~Bank() = default;
    void buyStreet(Player* buyer, StreetProperty* property, int turn);

    void claimFreeProperty(Player* claimer, Property* property, int turn);

    // Property Liquidation  (player -> bank)
    int sellPropertyToBank(Player* seller, Property* property, int turn);

    void mortgageProperty(Player* owner, Property* property, int turn);

    void unmortgageProperty(Player* owner, Property* property, int turn);

    void buildOnProperty(Player* owner, StreetProperty* property, int turn);

    int getRailroadRent(int ownedCount) const;

    void collectJailFine(Player* payer, int turn);

    vector<Property*> settleBankruptcyToBank(Player* debtor, int turn);

    void settleBankruptcyToPlayer(Player* debtor, Player* creditor, int turn);

    // ----------------------------------------------------------------
    // Getters
    // ----------------------------------------------------------------
    int getGoSalary()  const;
    int getJailFine()  const;
};
