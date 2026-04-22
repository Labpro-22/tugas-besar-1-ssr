#pragma once

#include <string>
#include <vector>

#include "Board.hpp"
#include "Player.hpp"
#include "Dice.hpp"
#include "Card.hpp"
#include "TransactionLogger.hpp"
#include "Property.hpp"

class Board;

class Game {
private:
    
    Board *board;
    std::vector<Player *> players;
    std::vector<int> turnOrder;
    int currentTurnIndex;
    int currentTurn;
    int maxTurn;
    int startingBalance;
    std::array<Dice, 2> dices;
    TransactionLogger *Logger;

    using GameCommand = std::function<int(const std::vector<std::string>&)>;
    std::unordered_map<std::string, GameCommand> gameCommands;

public:
    Game();
    Game(std::string &configPath);

    ~Game() = default;
    
    void startGame();
    void runCommand(std::string &command);

    void nextTurn();
    Player* getCurrentPlayer();
    bool checkWinCondition();
    void handleBankruptcy(Player* debtor, Player* creditor, int amount);
    void handleAuction(Property* property, Player* triggeredBy);
    void distributeSalary(Player* player);
    void removePlayer(Player* player);
    void endGame();
};