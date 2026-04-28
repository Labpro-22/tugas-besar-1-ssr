#include "GameSession.hpp"
#include "Tile.hpp"
#include "Property.hpp"
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
}



GameSession::GameSession(std::string &saveDataDir, GameConfig *config) : 
    startingBalance(config ? config->initialBalance : 1000), maxTurn(config ? config->maxTurn : 40), currentTurn(1),
    currentPlayerIndex(0), isRunning(true), hasCurrentPlayerActed(false), logger(new TransactionLogger()), config(config)
{
    LoadHandler loader;
    loader.loadSave(saveDataDir, this);

    if(fundDeck.isEmpty()) initializeDefaultFund();
    if(oppoturnityDeck.isEmpty()) initializeDefaultOppoturnity();
    if(skillDeck.isEmpty()) initializeDefaultSkill();
}



void GameSession::initializeDefaultSkill() {
    for(int i = 0; i < 4; i++) skillDeck.addCard(new MoveCard("Move" + std::to_string(i)));

    for(int i = 0; i < 3; i++) skillDeck.addCard(new DiscountCard("Discount" + std::to_string(i), 1));
    
    for(int i = 0; i < 2; i++) {
        skillDeck.addCard(new ShieldCard("Shield" + std::to_string(i), 1));
        skillDeck.addCard(new TeleportCard("Teleport" + std::to_string(i)));
        skillDeck.addCard(new LassoCard("Lasso" + std::to_string(i)));
        skillDeck.addCard(new DemolitionCard("Demolition" +std::to_string(i)));
    }
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
    delete config->board;
    for(Player* p : players) delete p;
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
    assignSkillCard();

    while(isRunning){
        Player* currentPlayer = getCurrentPlayer();

        std::cout << "\n[Turn " << currentTurn << "] Giliran " << currentPlayer->getUsername() << " (M" << currentPlayer->getMoney() << ")\n> ";
        if(currentPlayer->isJailed()){
            handleJail(currentPlayer);
            continue;
        }

        std::string line;
        if(!std::getline(std::cin, line)) break;
        runCommand(line);

        if(currentTurn > maxTurn || hasWinner()) endGame();
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
        handleGadai(currentPlayer);
    }
    else if(command == "TEBUS") {
        hasCurrentPlayerActed = true;
        handleTebus(currentPlayer);
    }
    else if(command == "BANGUN") {
        hasCurrentPlayerActed = true;
        handleBangun(currentPlayer);
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
                logger->log(currentTurn, currentPlayer->getUsername(), "SIMPAN", saveFileName);
                SaveHandler saver(saveFileName);
                saver.save(this);
                std::cout << "Permainan berhasil disimpan ke data/" << saveFileName << "\n";
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
    isRunning = false; 

    Player *winner = nullptr;
    for(auto p : players) if(p->isActive()) winner = p;

    if (winner) std::cout << "Pemenang: " << winner->getUsername() << "\n\n";
}


void GameSession::handleJail(Player *currentPlayer){

    std::cout << currentPlayer->getUsername() << " di penjara! Silahkan ambil pilihan:\n1. Bayar M" << config->jailFine << "\n2. Lempar dadu\n3. Kartu Freedom\n";
    
    int choice = currentPlayer->chooseInput({1, 2, 3});
    if(choice == 1){
        if(currentPlayer->canAfford(config->jailFine)){
            currentPlayer->deductMoney(config->jailFine);
            currentPlayer->setStatus(PlayerStatus::ACTIVE);
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
            currentPlayer->setStatus(PlayerStatus::ACTIVE);
            nextTurn(dices.getDie1(), dices.getDie2()); 
            return;
        } 
        else{
            std::cout << "Pemain " << currentPlayer->getUsername() << " gagal mendapatkan dadu yang sama...\n";
            currentPlayer->incJailAttempts();
        }
    }
    else {
        for(Card *card : currentPlayer->getHand()){

            FreedomCard *freedomCard = dynamic_cast<FreedomCard*>(card);
            if(freedomCard){
                freedomCard->use(currentPlayer);
                std::cout << "Pemain " << currentPlayer->getUsername() << " berhasil bebas dari penjara setelah menggunakan kartu!\n";
            }
            else{
                std::cout << "Pemain " << currentPlayer->getUsername() << " tidak memiliki kartu untuk bebas dari penjara!\n";
                currentPlayer->incJailAttempts();
            }
        }
    }

    if(currentPlayer->getJailAttempts() == 3){
        currentPlayer->setStatus(PlayerStatus::ACTIVE);
        std::cout << "Pemain " << currentPlayer->getUsername() << " telah menempuh hukuman 3 tahun penjara, silahkan keluar dari penjara!\n";
        currentPlayer->setJailAttempts(0);
    }

    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    if(currentPlayerIndex == 0) currentTurn++;
    beginCurrentPlayerTurn();
}


void GameSession::handleAuction(Property *property, Player *player){
    vector<Player *> activePlayer;
    for(Player *p: players) if(player->isActive()) activePlayer.push_back(p);

    Auction auction(property, activePlayer, currentPlayerIndex, logger, currentTurn);
    auction.run();
}


void GameSession::handleGadai(Player* player) {
    std::vector<Property*> owned; std::vector<int> choices;
    for (size_t i = 0; i < player->getAllProperties().size(); ++i) {
        if (player->getAllProperties()[i]->getStatus() == PropertyStatus::OWNED) {
            owned.push_back(player->getAllProperties()[i]);
            choices.push_back(i + 1);
            std::cout << i + 1 << ". " << owned.back()->getName() << "\n";
        }
    }
    if (choices.empty()) { std::cout << "Tidak ada properti untuk digadaikan.\n"; return; }
    int choice = player->chooseInput(choices);
    Property* p = player->getAllProperties()[choice-1];
    p->mortgage(); player->addMoney(p->getMortgageValue());
    logger->log(currentTurn, player->getUsername(), "GADAI", p->getCode());
}



void GameSession::handleTebus(Player* player) {
    std::vector<Property*> mortgaged; std::vector<int> choices;
    for (size_t i = 0; i < player->getAllProperties().size(); ++i) {
        if (player->getAllProperties()[i]->getStatus() == PropertyStatus::MORTGAGED) {
            mortgaged.push_back(player->getAllProperties()[i]);
            choices.push_back(i + 1);
            std::cout << i + 1 << ". " << mortgaged.back()->getName() << "\n";
        }
    }
    if (choices.empty()) { std::cout << "Tidak ada properti yang digadaikan.\n"; return; }
    int choice = player->chooseInput(choices);
    Property* p = player->getAllProperties()[choice-1];
    if (player->canAfford(p->getPrice())) {
        player->deductMoney(p->getPrice());
        p->unmortgage();
        logger->log(currentTurn, player->getUsername(), "TEBUS", p->getCode());
    } else std::cout << "Uang tidak cukup.\n";
}



void GameSession::handleBangun(Player* player) {
    std::vector<StreetProperty*> eligible; 
    std::vector<int> choices;
    for (size_t i = 0; i < player->getAllProperties().size(); ++i) {
        if (auto* sp = dynamic_cast<StreetProperty*>(player->getAllProperties()[i])) {
            if (sp->canBuild()) { eligible.push_back(sp); choices.push_back(i + 1); std::cout << i + 1 << ". " << sp->getName() << "\n"; }
        }
    }
    if (choices.empty()) { std::cout << "Tidak ada properti untuk dibangun.\n"; return; }
    int choice = player->chooseInput(choices);
    StreetProperty* sp = eligible[choice-1];
    
    int cost = sp->getBuildCost();
    if (player->getDiscount() > 0) {
        cost = static_cast<int>(cost * (1.0f - player->getDiscount() / 100.0f));
        std::cout << "Mendapatkan diskon " << player->getDiscount() << "%! Biaya bangun menjadi M" << cost << ".\n";
    }

    if (player->canAfford(cost)) {
        player->deductMoney(cost);
        sp->build();
        logger->log(currentTurn, player->getUsername(), "BANGUN", sp->getCode());
    } else std::cout << "Uang tidak cukup.\n";
}



void GameSession::handleBankruptcy(Player* debtor, Player* creditor, int amount) {
    std::cout << debtor->getUsername() << " bangkrut!\n";
    debtor->setStatus(PlayerStatus::BANKRUPT);
    for (auto p : debtor->getAllProperties()) if (creditor) creditor->addProperty(p);
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
    if (hand.empty()) { std::cout << "Tidak ada kartu.\n"; return; }
    std::vector<int> choices;
    for (size_t i = 0; i < hand.size(); ++i) { choices.push_back(i + 1); std::cout << i+1 << ". " << hand[i]->skillName << "\n"; }
    int choice = player->chooseInput(choices);
    hand[choice-1]->use(player);
    skillDeck.addCard(hand[choice-1]);
    player->removeCard(choice-1);
}



void GameSession::assignSkillCard() {
    for (Player* p : players) {
        if (p->isActive() && p->getCardCount() < 3) {
            try {
                SkillCard* card = skillDeck.draw(true);
                std::cout << "Pemain " << p->getUsername() << " mendapatkan kartu spesial: " << card->skillName << '\n'; 
                p->addCard(card);
            } catch (const AppException& e) {
                std::cout << "Gagal membagikan kartu: " << e.what() << "\n";
            }
        }
    }
}
