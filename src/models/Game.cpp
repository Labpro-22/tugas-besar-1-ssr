#include <iostream>

#include "Game.hpp"
#include "Tile.hpp"

Game::Game(std::string &configPath) {
    // LoadHandler *loadHandler(configPath);
    // loadHandler->load(this);
}


Game::Game() : 
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

        while(isRunning){
            std::string command;
            if(command == "LEMPAR_DADU"){
                nextTurn();
            }
            
        }

    }
}


int Game::runCommand(std::string &command){
    // std::istringstream iss(text);
    // std::vector<std::string> args;
    // std::string command, arg;

    // iss >> command;

    // while(iss >> arg){
    //     args.push_back(arg);
    // }

    // return gameCommands[command](args);
    return 0;
}


void Game::nextTurn() {

    Player *currentPlayer = getCurrentPlayer();

    dices.roll();

    int die1 = dices.getDie1(), die2 = dices.getDie2();
    bool isDouble = dices.isDouble();
    int total = die1 + die2;

    std::cout << "Mengocok dadu...\n";
    std::cout << "Hasil: " << die1 << " + " << die2  << '\n';
    std::cout << "Total: " << total << '\n';
    if(die1 == die2){
        std::cout << "Didapatkan dadu double!\n";
        currentPlayer->incConsecutiveDouble();

        if(currentPlayer->aboveSpeedLimit()){
            std::cout << "Pemain '" << currentPlayer->getUsername() << "' telah melanggar batas kecepatan! Silahkan masuk penjara :)\n";
            currentPlayer->moveTo(board, board->getJailIndex());
        }
        
    }
    
    std::cout << "Memajukan bidak pemain '" << currentPlayer->getUsername() << "' sebanyak " << total << " petak...\n";
    currentPlayer->moveForward(board, total);
    

    if(!isDouble){
        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();

        if(!currentPlayerIndex == 0){
            turnPlayed++;
            if(turnPlayed > maxTurn || hasWinner()){
                endGame();
            }
        }
    }

}


Player* Game::getCurrentPlayer() { return players[currentPlayerIndex]; }


bool Game::hasWinner() {
    int activeCount = 0;
    for(Player *p : players) if(p->isActive()) activeCount++;

    return (activeCount == 1);
}


void Game::endGame() {
    isRunning = false;
}
