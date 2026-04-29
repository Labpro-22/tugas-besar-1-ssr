#pragma once

#include <string>
#include <vector>

#include "GameConfig.hpp"
#include "Board.hpp"
#include "Dice.hpp"
#include "Card.hpp"
#include "Bank.hpp"

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

    Bank *bank;

    CardDeck<ActionCard> fundDeck;
    CardDeck<ActionCard> oppoturnityDeck;
    CardDeck<SkillCard> skillDeck;
    
    void initializeDefaultOppoturnity();
    void initializeDefaultFund();
    void initializeDefaultSkill();
    
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
    GameConfig *getConfig(){ return config; }
    Bank *getBank(){ return bank; }
    TransactionLogger *getLogger(){ return logger; }
    CardDeck<ActionCard> &getFundDeck(){ return fundDeck; }
    CardDeck<ActionCard> &getOppoturnityDeck(){ return oppoturnityDeck; }
    CardDeck<SkillCard> &getSkillDeck(){ return skillDeck; }

    std::vector<Player*> &getPlayers(){ return players; }
    void setPlayers(std::vector<Player*> &p){ players = p; }

    Player* getCurrentPlayer();
    int getCurrentTurn() const { return currentTurn; }
    int getCurrentPlayerIndex() const { return currentPlayerIndex; }
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

    void updateJailState(Player *currentPlayer);
    void updateFestivalState();
    void useSkillCard(Player* player);
    void assignSkillCard();

    void addFundCard(ActionCard *card){ fundDeck.addCard(card); }
    void addOppoturnityCard(ActionCard *card){ oppoturnityDeck.addCard(card); }
    void addSkillCard(SkillCard *card){ skillDeck.addCard(card); }

    ActionCard* drawFundCard(bool removeCard) { return fundDeck.draw(removeCard); }
    ActionCard* drawOppoturnityCard(bool removeCard) { return oppoturnityDeck.draw(removeCard); }
    SkillCard* drawSkillCard(bool removeCard) { return skillDeck.draw(removeCard); }

    void log(std::string action, std::string detail);

    void removePlayer(Player* player);
    void endGame();
};
