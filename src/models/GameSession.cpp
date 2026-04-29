#include "GameSession.hpp"
#include "Tile.hpp"
#include "Property.hpp"
#include "Bank.hpp"
#include "TransactionLogger.hpp"
#include "SaveLoadHandler.hpp"
#include "Auction.hpp"
#include "GameApp.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>


GameSession::GameSession(GameConfig *config) : 
    startingBalance(config ? config->initialBalance : 1000), maxTurn(config ? config->maxTurn : 40), currentTurn(1),
    currentPlayerIndex(0), isRunning(true), hasCurrentPlayerActed(false), logger(new TransactionLogger()), config(config)
{
    config->board->initializeDefault();
    
    bank = new Bank(config->goSalary, config->jailFine, config->railroadRentTable, config->utilityMultiplierTable);

    int count;
    std::cout << "Masukkan jumlah pemain (2-4): ";
    while (!(std::cin >> count) || count < 2 || count > 4) {
        std::cout << "Input tidak valid (2-4): ";
        std::cin.clear(); 
        std::cin.ignore(1000, '\n');
    }

    std::cin.ignore(1000, '\n');
    
    std::cout << "Silahkan daftarkan username pemain,\n";
    for(int i = 0; i < count; ++i) {
        std::cout << "Username pemain " << i + 1 << ": ";
        std::string name; 

        std::getline(std::cin, name);
        players.push_back(new HumanPlayer(name, i, startingBalance));
    }
    std::cout << "\n[PRESS ENTER TO START]\n";

    std::random_device rd; std::mt19937 g(rd());
    std::shuffle(players.begin(), players.end(), g);

    initializeDefaultOppoturnity();
    initializeDefaultFund();
    initializeDefaultSkill();

    this->log("LOAD", "Permainan baru berhasil dimuat");
}



GameSession::GameSession(std::string &saveDataDir, GameConfig *config) : 
    startingBalance(config ? config->initialBalance : 1000), maxTurn(config ? config->maxTurn : 40), currentTurn(1),
    currentPlayerIndex(0), isRunning(true), hasCurrentPlayerActed(false), logger(new TransactionLogger()), config(config)
{    
    bank = new Bank(config->goSalary, config->jailFine, config->railroadRentTable, config->utilityMultiplierTable);

    LoadHandler loader;
    loader.loadSave(saveDataDir, this);
    this->log("LOAD", "Permainan berhasil dimuat melalui data dari " + saveDataDir);

    initializeDefaultFund();
    initializeDefaultOppoturnity();
    initializeDefaultSkill();
}



void GameSession::initializeDefaultSkill() {

    std::map<std::string, int> countMap = skillDeck.getCardCounts();
    if(countMap["Unknown"] > 0) throw ResourceException("Invalid SkillCard");
    if(countMap["MoveCard"] > 4) throw ResourceException("Invalid number of MoveCard");
    if(countMap["DiscountCard"] > 3) throw ResourceException("Invalid number of DiscountCard");
    if(countMap["ShieldCard"] > 2) throw ResourceException("Invalid number of ShieldCard");
    if(countMap["TeleportCard"] > 2) throw ResourceException("Invalid number of TeleportCard");
    if(countMap["LassoCard"] > 2) throw ResourceException("Invalid number of LassoCard");
    if(countMap["DemolitionCard"] > 2) throw ResourceException("Invalid number of DemolitionCard");
    if(countMap["FreedomCard"] > 2) throw ResourceException("Invalid number of FreedomCard");

    for(int i = 0; i < 2 - countMap["MoveCard"]; i++) 
        skillDeck.addCardToDiscarded(new MoveCard("Move" + std::to_string(i)));

    for(int i = 0; i < 3 - countMap["DiscountCard"]; i++)
        skillDeck.addCardToDiscarded(new DiscountCard("Discount" + std::to_string(i), 1));
    
    for(int i = 0; i < 2 - countMap["ShieldCard"]; i++) 
        skillDeck.addCardToDiscarded(new ShieldCard("Shield" + std::to_string(i), 1));
    
    for(int i = 0; i < 2 - countMap["TeleportCard"]; i++) 
        skillDeck.addCardToDiscarded(new TeleportCard("Teleport" + std::to_string(i)));

    for(int i = 0; i < 2 - countMap["LassoCard"]; i++) 
        skillDeck.addCardToDiscarded(new LassoCard("Lasso" + std::to_string(i)));

    for(int i = 0; i < 2 - countMap["DemolitionCard"]; i++) 
        skillDeck.addCardToDiscarded(new DemolitionCard("Demolition" +std::to_string(i)));

    for(int i = 0; i < 2 - countMap["FreedomCard"]; i++) 
        skillDeck.addCardToDiscarded(new FreedomCard("Freedom" +std::to_string(i)));


    skillDeck.shuffle();
}


void GameSession::initializeDefaultFund() {
    fundDeck.addCard(new GratificationCard("GRATIFICATION", 200));
    fundDeck.addCard(new BirthdayGiftCard("BIRTHDAY", 100));
    fundDeck.addCard(new DoctorFeeCard("DOCTOR", 700));

    fundDeck.shuffle();
}


void GameSession::initializeDefaultOppoturnity() {
    oppoturnityDeck.addCard(new GoToStationCard("GO_STATION"));
    oppoturnityDeck.addCard(new MoveBackCard("MOVE_BACk", 3));
    oppoturnityDeck.addCard(new GoToJailCard("GO_JAIL"));

    oppoturnityDeck.shuffle();
}
   



void GameSession::beginCurrentPlayerTurn() {
    hasCurrentPlayerActed = false;
    if (!players.empty() && currentPlayerIndex >= 0 && currentPlayerIndex < static_cast<int>(players.size())) {
        players[currentPlayerIndex]->hasRolledDice = false;
        players[currentPlayerIndex]->hasUsedSkill = false;
    }
}



GameSession::~GameSession() {
    for(Player* p : players) delete p;
    delete bank;
    delete logger;
}



Player* GameSession::getCurrentPlayer() {
    if (players.empty()) {
        throw GameException("GameSession", "Tidak ada pemain dalam sesi permainan.");
    }
    if (currentPlayerIndex < 0 || currentPlayerIndex >= static_cast<int>(players.size())) {
        throw GameException("GameSession", "Indeks pemain aktif tidak valid.");
    }
    return players[currentPlayerIndex];
}



void GameSession::startGame(){

    std::cin.ignore(1000, '\n');
    std::cout << "Selamat datang di Nimonspoli!\n";

    config->board->printBoard();

    bool foundNotEmptyHand = false;
    for(Player *p : players){
        if(!p->getHand().empty()) foundNotEmptyHand = true;
    }
    if(!foundNotEmptyHand) assignSkillCard();

    while(isRunning){
        Player* currentPlayer = getCurrentPlayer();

        std::cout << "\n[Turn " << currentTurn << "] Giliran " << currentPlayer->getUsername() << " (M" << currentPlayer->getMoney() << ")\n> ";
        if(currentPlayer->isJailed()){
            updateJailState(currentPlayer);
            continue;
        }

        std::string line;
        if(!std::getline(std::cin, line)) break;
        runCommand(line);

        if((maxTurn > 0 && currentTurn > maxTurn) || hasWinner()) endGame();
    }
}



int GameSession::runCommand(std::string &text){
    
    Player* currentPlayer = getCurrentPlayer();

    std::istringstream iss(text);
    std::vector<std::string> args;
    std::string command, arg;

    iss >> command;
    while(iss >> arg) args.push_back(arg);

    if (command.empty()) return 0;

    if(command == "CETAK_PAPAN") config->board->printBoard();
    else if(command == "LEMPAR_DADU"){
        if (currentPlayer->hasRolledDice) std::cout << "Sudah melempar dadu!\n";
        else {
            hasCurrentPlayerActed = true;
            nextTurn();
        }
    }
    else if(command == "ATUR_DADU"){
        if (args.size() == 2) {
            hasCurrentPlayerActed = true;
            nextTurn(std::stoi(args[0]), std::stoi(args[1]));
        }
        else std::cout << "Penggunaan: ATUR_DADU X Y\n";
    }
    else if(command == "CETAK_AKTA"){
        if (args.empty()) std::cout << "Gunakan: CETAK_AKTA <kode>\n";
        else {
            try {
                Tile* tile = config->board->getTileByCode(args[0]);
                auto* pt = dynamic_cast<PropertyTile*>(tile);
                if (pt && pt->property) {
                    std::stringstream ss; pt->property->printCertificate(ss); std::cout << ss.str();
                } else std::cout << "Petak bukan properti.\n";
            } catch (AppException &) { std::cout << "Kode tidak ditemukan.\n"; }
        }
    }
    else if(command == "CETAK_PROPERTI") currentPlayer->printProperties();
    else if(command == "GADAI") {
        hasCurrentPlayerActed = true;
        bank->handleMortgage(currentPlayer);
    }
    else if(command == "TEBUS") {
        hasCurrentPlayerActed = true;
        bank->handleUnmortgage(currentPlayer);
    }
    else if(command == "BANGUN") {
        hasCurrentPlayerActed = true;
        bank->handleBuild(currentPlayer);
    }
    else if(command == "GUNAKAN_KEMAMPUAN") {
        if(currentPlayer->hasUsedSkill){
            std::cout << "Pemain " << currentPlayer->getUsername() << " sudah menggunakan kartu spesial, silahkan tunggu giliran selanjutnya!\n";
        }
        else{
            useSkillCard(currentPlayer);
            hasCurrentPlayerActed = true;
            currentPlayer->hasUsedSkill = true;
        }
    }
    else if(command == "SIMPAN") {
        if (!canSaveAtTurnStart()) {
            std::cout << "Simpan hanya boleh dilakukan di awal giliran sebelum aksi apa pun.\n";
        } else {
            std::string saveFileName;
            if (!args.empty()) saveFileName = args[0];
            else {
                std::cout << "Nama file save: ";
                std::cin >> saveFileName;
                std::cin.ignore(1000, '\n');
            }

            try {
                SaveHandler saver(saveFileName);
                saver.save(this);
                std::cout << "Permainan berhasil disimpan ke data/" << saveFileName << "\n";
                this->log("SAVE", "Pemain " + currentPlayer->getUsername() + " berhasil menyimpan sesi permainan ke " + saveFileName);
            } catch (AppException &e) {
                std::cout << "Gagal menyimpan permainan: " << e.what() << "\n";
            }
        }
    }
    else if(command == "CETAK_LOG") {
        int limit = args.empty() ? -1 : std::stoi(args[0]);
        if (limit == -1) logger->showRecentEntry(logger->entryCount());
        else logger->showRecentEntry(limit);
    }
    else std::cout << "Perintah tidak dikenali.\n";
    return 0;
}



void GameSession::nextTurn() {
    dices.roll();
    nextTurn(dices.getDie1(), dices.getDie2());
}



void GameSession::nextTurn(int die1, int die2) {

    if(die1 < 1 || die1 > 6 || die2 < 1 || die2 > 6){
        std::cout << "Nilai dadu tidak valid! (" << die1 << ", " << die2 << "), tolong masukkan bilangan dalam rentang 1-6!\n";
        return;
    }

    Player *currentPlayer = getCurrentPlayer();
    dices.setManual(die1, die2);

    int total = die1 + die2;
    std::cout << "Nilai dadu: " << die1 << " + " << die2 << " = " << total << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    
    if(die1 == die2){
        currentPlayer->incConsecutiveDouble();
        if(currentPlayer->aboveSpeedLimit()){
            currentPlayer->setStatus(PlayerStatus::JAILED);
            currentPlayer->setPosition(config->board->getJailIndex());
            std::cout << "Pemain " << currentPlayer->getUsername() << " melanggar batas kecepatan! Hukumannya penjara selama 3 tahun...\n";
            logger->log(currentTurn, currentPlayer->getUsername(), "Lempar", "Melanggar batas kecepatan, masuk ke penjara");
            currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
            return;
        }
    }
    int currPos = currentPlayer->getPosition();
    while(total > 1){
        currPos = (currPos + 1) % 40;
        currentPlayer->setPosition(currPos);
        Tile *tile = config->board->getTile(currPos);
        tile->onPassed(currentPlayer);
        total--;
    }

    currPos = (currPos + 1) % 40;
    currentPlayer->setPosition(currPos);
    Tile *tile = config->board->getTile(currPos);
    tile->onLanded(currentPlayer);
    
    if(die1 == die2){        
        std::cout << "Pemain " << currentPlayer->getUsername() << " berhasil mendapatkan dadu yang sama, silahkan jalankan giliran lagi.";
    }
    else{
        currentPlayer->resetConsecutiveDouble();
        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
        if(currentPlayerIndex == 0){
            currentTurn++;
            assignSkillCard();
            updateFestivalState();
        }
        beginCurrentPlayerTurn();
    }

    logger->log(currentTurn, currentPlayer->getUsername(), "Lempar", std::to_string(die1) + "+" + std::to_string(die2) + "=" + std::to_string(total) + " mendarat di " + tile->getName());
}



bool GameSession::hasWinner() {
    int bankrupt = 0;
    for(auto p : players) if(p->isBankrupt()) bankrupt++;
    return (bankrupt == players.size() - 1);
}



void GameSession::endGame() { 
    std::cout << "\n\nTelah ditemukan pemenang dari permainan...\n";
    isRunning = false; 

    std::vector<Player*> winner;
    int maxMoney = 0, maxCardCount = 0;
    for(Player *p : players){
        if(!p->isBankrupt()){
            if(p->getMoney() > maxMoney) maxMoney = p->getMoney();
            if(p->getCardCount() > maxCardCount) maxCardCount = p->getCardCount();
        }
    }

    for(Player *p: players){
        if(!p->isBankrupt()){
            if(p->getMoney() == maxMoney && p->getCardCount() == maxCardCount) winner.push_back(p);
        }
    }

    std::cout << "\n==================================================================\n";
    std::cout << "Pemenang: ";
    for(int i = 0; i < winner.size(); i++){
        std::cout << "   " << i+1 << ".) " << winner[i]->getUsername()  << " (Player " << winner[i]->getPlayerIndex() << ")\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
    std::cout << "==================================================================\n";
    std::cout << "    ⡴⢋⡉⢧⠀⠀⢀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀    \n";
    std::cout << "⠀⠀⠀⠻⡁⢖⡾⠤⣠⡿⣄⣙⡄⠀⠀⠀⠀⠀⢀⣀⡤⠤⠤⢤⣒⡊⠉⠉⠉⠉⠉⠉⠉⠉⠉⠒⠢⠤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⢰⡏⢛⡶⣽⠈⢰⠀⠉⢯⡷⠛⠀⠀⠀⢀⡤⠚⠉⢀⣀⣠⢤⣀⣀⠉⠓⢄⠀⠀⠀⠀⣀⡤⠔⠒⠒⠒⠢⠭⣓⢤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠈⠙⠚⠒⢧⡀⠈⢦⢀⣸⠁⠀⠀⠀⣠⠋⠀⡠⠚⠉⠀⠀⠀⠀⠈⠑⢦⡀⠳⡄⡤⠊⠁⣀⠤⠔⠒⠒⠤⣄⡀⠙⢯⡢⡀⠀⠀⠀⠀⠀⢀⣀⡀⠀⠀⣴⡋⠙⡆⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠉⡗⠚⠈⢈⡣⠀⠀⢸⠃⠀⡜⠁⠀⠀⠀⠀⠀⣀⡀⠀⠀⢱⡀⠙⠀⣠⠎⠁⠀⠀⠀⠀⠀⠀⠙⢦⡀⠱⡘⢆⠀⠀⠀⢠⡏⠠⠾⠤⠴⣧⣉⡼⣁⣀⡀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠳⢶⠚⠉⢇⠀⠀⢸⠀⢰⠃⠀⠀⠀⠀⣰⣛⡧⣍⠳⡀⠀⣇⠀⣰⠃⠀⢀⣠⠤⢤⡀⠀⠀⠀⠀⢳⠀⢷⠾⣆⠀⠀⠀⠙⢾⠀⠀⣰⠇⠙⢿⠧⣠⡇\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠘⡆⠀⠘⡄⠀⣾⠀⠘⡆⠀⠀⠀⠀⢳⡙⠶⠜⣠⠇⢀⡏⠀⣏⠀⠠⡿⢿⠓⣦⢹⠀⠀⠀⠀⢸⡆⢸⠀⣿⡄⠀⠀⢀⡼⢧⡀⠁⠀⣠⠗⠋⠁⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⢳⠀⠀⢽⡀⡟⣇⠀⠱⡄⠀⠀⠀⠀⠉⠒⠚⠁⢀⡞⠀⠀⢻⡀⠀⠳⣌⠉⣁⠞⠀⠀⠀⠀⣼⠀⣸⠉⡇⣿⠀⠀⣹⢤⣀⠈⢹⠋⠁⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠈⡇⠀⠀⢣⡇⠈⢦⡀⠈⠳⢤⣀⣀⣀⣀⡠⠔⠋⢀⡜⣆⠀⠳⣄⠀⠀⠉⠀⠀⠀⠀⢀⡼⠁⢠⣧⣼⠢⣿⠀⣰⠃⠀⢨⠏⠋⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡀⠀⢸⠀⠀⠀⠙⠢⣄⣀⠀⠀⠀⠀⣀⡠⠖⠋⠀⠈⢢⡀⠈⠑⠢⠤⠤⠤⠤⠚⠋⢀⡴⠃⠀⠀⠀⢻⣰⠃⠀⢠⠏⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⢠⢧⡀⠀⠀⠀⠀⠀⠈⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠈⠓⠤⢄⣀⣀⣀⡠⠤⠖⠉⠀⠀⠀⠀⠀⢸⠃⠀⢀⠎⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣸⡀⠙⣄⠀⠀⠀⢸⢭⣹⣒⠒⠠⢤⢄⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡼⡆⢀⡞⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠳⣄⠈⢣⡀⠀⠈⢧⡀⠀⠉⠉⠉⠙⠒⠒⠊⠙⠛⠭⠭⠛⠯⠭⢭⡟⠀⠀⠀⠀⠀⢀⣠⠖⠋⢀⡇⡼⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠘⢦⠀⠙⣄⠀⠀⠙⠦⣤⠤⠤⠤⠤⣀⣀⣀⡀⠀⢀⣀⣀⡴⠋⠀⠀⠀⠀⣀⠴⠋⠀⢀⡴⢺⣱⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⠀⠀⠈⢧⠀⠘⢦⡀⠀⠀⠈⠉⠒⠒⠢⠤⠤⠤⠤⠒⠒⠋⠁⠀⠀⠀⠀⣠⠞⠁⠀⣠⠞⠉⠀⢸⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠀⠀⠈⣧⣾⣾⣯⠉⠉⠉⠉⠉⠉⠛⠛⠛⠒⠒⠒⠒⠒⠒⠢⠤⣤⣾⣧⡀⣠⠞⠁⠀⠀⠀⣸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⠀⠀⠀⠀⡏⢿⣿⠞⠀⠀⢠⢀⣀⣀⣀⣀⣀⣀⣀⣀⣀⡀⠀⠀⠸⣗⢿⡽⡷⠋⠀⠀⠀⠀⠀⢻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⢧⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠃⠀⠀⠀⠈⠳⢾⠇⠀⠀⠀⠀⠀⠀⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢈⡆⠀⠀⠀⢸⠀⠀⠀⠀⠀⠘⡆⠀⠀⠀⠀⠀⠀⠀⠀⡼⠀⠀⠀⠀⠀⠀⣸⠀⠀⠀⠀⠀⠀⢀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⣿⠉⠒⡾⠀⠀⠀⠀⠀⠀⠘⠦⠄⣀⣀⣀⡀⠤⠞⠁⠀⠀⠀⠀⠀⠀⢸⡶⠶⠶⠶⢶⣶⠞⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢧⢸⣀⡴⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢳⡀⠀⢀⡾⠛⣸⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢏⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠲⠾⠥⡼⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠳⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡴⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠒⠦⠤⣀⣀⣀⡀⠀⠀⠀⢸⣸⡀⠀⠀⠀⠀⠀⢀⣀⣀⣠⠤⠒⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⡍⠉⠉⠉⠉⢹⠍⠉⠉⠉⠉⢉⡝⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠤⠟⠒⠒⠒⠒⠚⣷⠒⠒⠒⠒⢿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣅⣀⣀⣀⣀⣤⣤⣴⠷⣦⣤⣀⣀⣀⣸\n";
    std::cout << "==================================================================\n\n";
}


void GameSession::updateJailState(Player *currentPlayer){

    std::cout << currentPlayer->getUsername() << " di penjara! Silahkan ambil pilihan:\n1. Bayar M" << config->jailFine << "\n2. Lempar dadu\n3. Kartu Freedom\n";
    
    int choice = currentPlayer->chooseInput({1, 2, 3});
    if(choice == 1){
        if(currentPlayer->canAfford(config->jailFine)){
            currentPlayer->deductMoney(config->jailFine);
            currentPlayer->setStatus(PlayerStatus::ACTIVE);
            currentPlayer->setJailAttempts(0);
            currentPlayer->resetConsecutiveDouble();
            std::cout << "Pemain " << currentPlayer->getUsername() << " berhasil membayar denda dan sekarang bebas dari penjara!\n";
        }
        else{
            std::cout << "Pemain " << currentPlayer->getUsername() << " tidak memiliki cukup uang untuk membayar denda!\n";
            currentPlayer->incJailAttempts();
        }
    } 
    else if(choice == 2) {
        dices.roll();
        if (dices.isDouble()) {
            std::cout << "Pemain " << currentPlayer->getUsername() << " berhasil mendapatkan dadu yang sama! Silahkan keluar dari penjara...\n";
            currentPlayer->resetConsecutiveDouble();
            currentPlayer->setStatus(PlayerStatus::ACTIVE);
        } 
        else{
            std::cout << "Pemain " << currentPlayer->getUsername() << " gagal mendapatkan dadu yang sama...\n";
            currentPlayer->incJailAttempts();
        }
    }
    else {
        std::vector<SkillCard*> hand = currentPlayer->getHand();

        int idx = -1;
        for(int i = 0; i < hand.size(); i++){
            if(dynamic_cast<FreedomCard*>(hand[i])){
                idx = i;
                break;
            }
        }

        if(idx != -1){
            hand[idx]->use(currentPlayer);

            if(auto pos = hand[idx]->skillName.find(" ("); pos != std::string::npos) hand[idx]->skillName.erase(pos);
            skillDeck.addCardToDiscarded(hand[idx]);

            currentPlayer->removeCard(idx);
            std::cout << "Pemain " << currentPlayer->getUsername() << " berhasil bebas dari penjara setelah menggunakan kartu!\n";
        }
        else{
            std::cout << "Pemain " << currentPlayer->getUsername() << " tidak memiliki kartu untuk bebas dari penjara!\n";
            currentPlayer->incJailAttempts();
        }
    }

    if(currentPlayer->getStatus() == PlayerStatus::JAILED && currentPlayer->getJailAttempts() == 3){
        currentPlayer->setStatus(PlayerStatus::ACTIVE);
        std::cout << "Pemain " << currentPlayer->getUsername() << " telah menempuh hukuman 3 tahun penjara, silahkan keluar dari penjara!\n";
        currentPlayer->setJailAttempts(0);
        currentPlayer->resetConsecutiveDouble();
    }

    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    if(currentPlayerIndex == 0) currentTurn++;
    beginCurrentPlayerTurn();
}


void GameSession::updateFestivalState() {

    for(Player* player : players){
        for(Property *prop : player->getAllProperties()){
            prop->decrementFestival();
        }
    }
}


void GameSession::useSkillCard(Player* player) {
    auto& hand = player->getHand();
    if (hand.empty()) { std::cout << "Tidak ada kartu spesial yang dapat digunakan.\n"; return; }

    std::vector<int> choices;
    for (size_t i = 0; i < hand.size(); ++i) { choices.push_back(i + 1); std::cout << i+1 << ". " << hand[i]->skillName << "\n"; }
    int choice = player->chooseInput(choices);
    
    hand[choice-1]->use(player);
    if(auto pos = hand[choice-1]->skillName.find(" ("); pos != std::string::npos) hand[choice-1]->skillName.erase(pos);

    skillDeck.addCard(hand[choice-1]);
    player->removeCard(choice-1);
}



void GameSession::assignSkillCard() {
    std::cout << "\n\n============= PEMBAGIAN KARTU SPESIAL =============\n\n";
    for (Player* p : players) {
        if (p->isActive()) {
            try {

                SkillCard* card = skillDeck.draw(true); 

                if(auto s = dynamic_cast<MoveCard*>(card)){
                    s->setDistance(std::rand() % 30 + 5);
                }   

                if(auto s = dynamic_cast<DiscountCard*>(card)){
                    s->setDiscount(std::rand() % 80 + 10);
                }

                if(p->getCardCount() == 3){
                    std::cout << "Pemain " << p->getUsername() << " akan mendapatkan kartu spesial: " << card->skillName << '\n'; 
                    std::cout << "Tetapi pemain sudah memiliki 3 kartu spesial:\n";

                    int idx = 1;
                    for(SkillCard *card : p->getHand()){
                        std::cout << idx << ".) " << card->skillName << '\n';
                        idx++;
                    }

                    std::string in;
                    std::cout << "Buang salah satu kartu yang telah dimiliki untuk mendapatkan kartu baru? (y/n): ";
                    std::cin >> in;
                    while(in != "y" && in != "Y" && in != "n" && in != "N"){
                        std::cout << "Masukan tidak valid...\n";
                        std::cout << "Buang salah satu kartu yang telah dimiliki untuk mendapatkan kartu baru? (y/n): ";
                        std::cin >> in;
                    }

                    if(in == "y" || in == "Y"){
                        int choice;
                        std::cout << "Pilih nomor kartu untuk dibuang: ";
                        std::cin >> choice;
                        while(choice <= 0 || choice > p->getCardCount()){
                            std::cout << "Masukan tidak valid...\n";
                            std::cout << "Pilih nomor kartu untuk dibuang: ";
                            std::cin >> choice;
                        }

                        SkillCard *discarded = p->removeCard(choice - 1);
                        std::cout << "Kartu " << discarded->skillName << " berhasil dibuang, dan pemain mendapat kartu " << card->skillName << '\n';
                        p->addCard(card);
                    }
                    else {
                        std::cout << "Kartu " << card->skillName << " tidak diterima oleh pemain...\n";
                        skillDeck.addCard(card);
                    }
                    
                    std::cout << "\n\n";
                }
                else{
                    std::cout << "Pemain " << p->getUsername() << " mendapatkan kartu spesial: " << card->skillName << '\n'; 
                    p->addCard(card);
                }

            } 
            catch (const AppException& e) {
                std::cout << "Gagal membagikan kartu: " << e.what() << "\n";
            }
        }
    }
}


void GameSession::log(std::string action, std::string detail){
    Player *currentPlayer = this->getCurrentPlayer();
    logger->log(currentTurn, currentPlayer->getUsername(), action, detail);  
}