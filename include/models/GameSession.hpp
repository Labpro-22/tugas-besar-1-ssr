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

    CardDeck<Card> deck;

public:
    GameSession(GameConfig *config);
    GameSession(std::string &saveDataDir, GameConfig *config);
    ~GameSession();
    
    void setMaxTurn(int max) { maxTurn = max; }
    void setStartingBalance(int bal) { startingBalance = bal; }
    void setCurrentTurn(int turn) { currentTurn = turn; }
    void setCurrentPlayerIndex(int index) { currentPlayerIndex = index; }

    Board *getBoard(){ return config->board; }
    GameConfig *getconfig(){ return config; }
    TransactionLogger *getLogger(){ return logger; }
    CardDeck<Card> &getDeck(){ return deck; }
    std::vector<Player*> &getPlayers(){ return players; }
    Player* getCurrentPlayer();

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
