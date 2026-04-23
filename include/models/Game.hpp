#pragma once

#include <string>
#include <vector>

#include "Board.hpp"
#include "Player.hpp"
#include "Dice.hpp"
#include "Card.hpp"
#include "TransactionLogger.hpp"
#include "Property.hpp"

class Player;
class Property;
class Board;

class Game {
private:
    
    Board *board;
    std::vector<Player *> players;
    std::vector<int> turnOrder;
    int currentPlayerIndex;
    int turnPlayed;
    int maxTurn;
    int startingBalance;
    Dice dices;
    TransactionLogger *Logger;
    bool isRunning;

    using GameCommand = std::function<int(const std::vector<std::string>&)>;
    std::unordered_map<std::string, GameCommand> gameCommands;

public:
    Game();
    Game(std::string &configPath);

    ~Game() = default;
    
    Board *getBoard(){ return board; }
    std::vector<Player*> &getPlayers(){ return players; }

    void startGame();
    int runCommand(std::string &command);

    void nextTurn();
    bool hasWinner();
    Player* getCurrentPlayer();
    int getLastDiceTotal(){ return dices.getTotal(); }
    bool checkWinCondition();
    void handleBankruptcy(Player* debtor, Player* creditor, int amount) {};
    void handleAuction(Property* property, Player* triggeredBy) {};
    void distributeSalary(Player* player);
    void removePlayer(Player* player);
    void endGame();
};