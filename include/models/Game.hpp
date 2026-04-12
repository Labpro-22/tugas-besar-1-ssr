#pragma once

#include <string>
#include <vector>
#include "Board.hpp"
#include "Player.hpp"
#include "Dice.hpp"
#include "CardDeck.hpp"
#include "ActionCard.hpp"
#include "SkillCard.hpp"
#include "TransactionLogger.hpp"
#include "Property.hpp"

class Game {
private:
    Board* board;
    std::vector<Player*> players;
    std::vector<int> turnOrder;
    int currentTurnIndex;
    int currentTurn;
    int maxTurn;
    int startingBalance;
    std::vector<Dice*> dices;
    CardDeck<ActionCard*>* chanceDeck;
    CardDeck<ActionCard*>* communityDeck;
    CardDeck<SkillCard*>* skillDeck;
    TransactionLogger* Logger;
    bool isRunning;
    
public:
    void startGame(std::string filename);
    void nextTurn();
    Player* getCurrentPlayer();
    bool checkWinCondition();
    void handleBankruptcy(Player* debtor, Player* creditor, int amount);
    void handleAuction(Property* property, Player* triggeredBy);
    void distributeSalary(Player* player);
    void removePlayer(Player* player);
    void endGame();
};