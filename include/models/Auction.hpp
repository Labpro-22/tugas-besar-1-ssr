#pragma once

#include <string>
#include <vector>

#include "Property.hpp"
#include "TransactionLogger.hpp"

class Auction {
private:
    Property* property;
    vector<Player*> activePlayers; 
    int triggerIndex;  
    TransactionLogger* logger;
    int gameTurn;    
    Player* winner;
    int finalBid;
    bool sold; 

    int currentHighBid;
    Player* currentHighBidder;
    int consecutivePasses;
    bool atLeastOneBid;

    vector<Player*> buildBidOrder() const;
    int requestAction(Player* player) const;

    void printAuctionState() const;

    void logBid(Player* bidder, int amount) const;
    void logPass(Player* passer) const;

    int requestForcedBid(Player* player) const;

public:
    Auction(Property* property,
            vector<Player*> activePlayers,
            int triggerPlayerIndex,
            TransactionLogger* logger,
            int gameTurn);

    ~Auction() = default;

    void runAuction();
};
