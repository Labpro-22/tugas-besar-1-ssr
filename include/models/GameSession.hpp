#pragma once

#include <string>
#include <vector>

#include "GameConfig.hpp"
#include "Board.hpp"
#include "Dice.hpp"
#include "Card.hpp"

class Player;
class Property;
class TransactionLogger;

class GameSession {
private:
    std::vector<Player *> players;
    int currentPlayerIndex;
    int currentTurn;
    int maxTurn;
    int startingBalance;
    Dice dices;
    TransactionLogger *logger;
    GameConfig *config;
    bool isRunning;
    bool hasCurrentPlayerActed;

    CardDeck<Card> deck;
    void initializeDefaultDeck();
    void beginCurrentPlayerTurn();

public:
    GameSession(GameConfig *config);
    GameSession(std::string &saveDataDir, GameConfig *config);
    ~GameSession();
    
    void setMaxTurn(int max) { maxTurn = max; if (config) config->maxTurn = max; }
    void setStartingBalance(int bal) { startingBalance = bal; }
    void setCurrentTurn(int turn) { currentTurn = turn; }
    void setCurrentPlayerIndex(int index) { currentPlayerIndex = index; }

    Board *getBoard(){ return config->board; }
    GameConfig *getconfig(){ return config; }
    TransactionLogger *getLogger(){ return logger; }
    CardDeck<Card> &getDeck(){ return deck; }
    std::vector<Player*> &getPlayers(){ return players; }
    Player* getCurrentPlayer();
    int getCurrentTurn() const { return currentTurn; }
    int getMaxTurn() const { return maxTurn; }
    bool canSaveAtTurnStart() const { return !hasCurrentPlayerActed; }

    void startGame();
    int runCommand(std::string &command);

    void nextTurn();
    void nextTurn(int die1, int die2);
    bool hasWinner();
    void addPlayer(Player *p);
    int getLastDiceTotal(){ return dices.getTotal(); }
    bool checkWinCondition();

    void handleBankruptcy(Player* debtor, Player* creditor, int amount);
    void handleAuction(Property* property, Player* triggeredBy);
    
    void handleGadai(Player* player);
    void handleTebus(Player* player);
    void handleBangun(Player* player);

    void useSkillCard(Player* player);
    void drawCard(Player* player);
    void distributeSalary(Player* player);
    void removePlayer(Player* player);
    void endGame();
};
