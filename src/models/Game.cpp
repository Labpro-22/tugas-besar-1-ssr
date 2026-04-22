#include <iostream>

#include "Game.hpp"

Game::Game(std::string &configPath) {
    LoadHandler *loadHandler(configPath);
    loadHandler->load(this);
}


Game::Game() 
: 
    board(new Board(40)),
    startingBalance(1000),
    maxTurn(40)
{
    const int DEFAULT_PLAYER_NUM = 4;
    const int DEFAULT_INITIAL_MONEY = 1000;
    const int DEFAULT_MAX_TURN = 40;

    for(int playerID = 1; playerID <= DEFAULT_PLAYER_NUM; playerID++){
        players.push_back(new HumanPlayer("Player #" + std::to_string(playerID), playerID, startingBalance));
    }
    
}


void Game::startGame(){

    if(GUI_ENABLED){
        std::cout << "[TODO] GUI Should be here...\n";
    }
    else{

        while(true){
            std::string command;
            
        }

    }
}


int Game::runCommand(std::string command){
    std::istringstream iss(text);
    std::vector<std::string> args;
    std::string command, arg;

    iss >> command;

    while(iss >> arg){
        args.push_back(arg);
    }

    return gameCommands[command](args);
}


/*
Game::~Game() {
    delete board;
    for(auto p : players) delete p;
    for(auto d : dices) delete d;

    delete chanceDeck;
    delete communityDeck;
    delete skillDeck;
    delete Logger;
}

void Game::startGame(std::string filename) {
    isRunning = true;
    currentTurnIndex = 0;
    currentTurn = 1;
    // Should call SaveLoadHandlir
}


void Game::nextTurn() {
    currentTurnIndex = (currentTurnIndex + 1) % players.size();
    if (currentTurnIndex == 0) currentTurn++;
    if (currentTurn > maxTurn || checkWinCondition()) endGame();
}

Player* Game::getCurrentPlayer() { return players[currentTurnIndex]; }

bool Game::checkWinCondition() {
    int activeCount = 0;
    for (auto p : players) if (p->isActive()) activeCount++;
    return activeCount <= 1;
}

void Game::handleBankruptcy(Player* debtor, Player* creditor, int amount) {
    // Handled by BankrutpcyManager
}

void Game::handleAuction(Property* property, Player* triggeredBy) {
    // Handled by AuctionManager
}

void Game::distributeSalary(Player* player) {
    // Handled by AuctionManager
}

void Game::removePlayer(Player* player) {
    // Handled by BankrutpcyManager
}

void Game::endGame() {
    isRunning = false;
}
*/