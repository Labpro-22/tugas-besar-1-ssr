#include "SaveLoadHandler.hpp"
#include "GameSession.hpp"
#include "Board.hpp"
#include "Property.hpp"
#include "Tile.hpp"
#include "AppException.hpp"
#include "TransactionLogger.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

LoadHandler::LoadHandler() {}


GameConfig *LoadHandler::loadConfiguration(std::string &configDir) {

    GameConfig *config = new GameConfig();

    std::string configPath = "config/" + configDir + "/";

    std::ifstream miscFile(configPath + "misc.txt");
    std::ifstream actionFile(configPath + "aksi.txt");
    std::ifstream propertyFile(configPath + "property.txt");
    std::ifstream railroadFile(configPath + "railroad.txt");
    std::ifstream utilityFile(configPath + "utility.txt");
    std::ifstream taxFile(configPath + "tax.txt");
    std::ifstream specialFile(configPath + "special.txt");

    if(!miscFile.is_open()) throw ResourceException("Unable to open " + configPath + "misc.txt");
    if(!actionFile.is_open()) throw ResourceException("Unable to open " + configPath + "aksi.txt");
    if(!propertyFile.is_open()) throw ResourceException("Unable to open " + configPath + "property.txt");
    if(!railroadFile.is_open()) throw ResourceException("Unable to open " + configPath + "railroad.txt");
    if(!utilityFile.is_open()) throw ResourceException("Unable to open " + configPath + "utility.txt");
    if(!taxFile.is_open()) throw ResourceException("Unable to open " + configPath + "tax.txt");
    if(!specialFile.is_open()) throw ResourceException("Unable to open " + configPath + "special.txt");

    
    if(!(miscFile >> config->maxTurn >> config->initialBalance)){
        std::stringstream buffer;
        buffer << miscFile.rdbuf();
        throw ResourceException("Invalid line '" + buffer.str() + "' at file " + configPath + "misc.txt");
    }

    if(!(taxFile >> config->pphFlat >> config->pphPercentage >> config->pbmFlat)){
        std::stringstream buffer;
        buffer << taxFile.rdbuf();
        throw ResourceException("Invalid line '" + buffer.str() + "' at file " + configPath + "tax.txt");
    }

    if(!(specialFile >> config->goSalary >> config->jailFine)){
        std::stringstream buffer;
        buffer << specialFile.rdbuf();
        throw ResourceException("Invalid line '" + buffer.str() + "' at file " + configPath + "special.txt");
    }


    std::string line;

    while(std::getline(actionFile, line)){
        if(line.empty()) continue;

        std::istringstream iss(line);

        int id; 
        std::string kode, nama, jenis, warna;
        if(!(iss >> id >> kode >> nama >> jenis >> warna)){
            throw ResourceException("Invalid line '" + line + "' at file " + configPath + "aksi.txt");
        }

        id--;
        if(id < 0 || id >= config->board->getTileCount()){
            throw ResourceException("Invalid line '" + line + "' at file " + configPath + "aksi.txt");
        }

        if(jenis == "SPESIAL"){
            if (kode == "GO") config->board->setTile(new StartTile(id, kode, nama, jenis, config->goSalary), id);
            else if (kode == "PEN") config->board->setTile(new JailTile(id, kode, nama, jenis), id);
            else if (kode == "PPJ") config->board->setTile(new GoToJailTile(id, kode, nama, jenis), id);
            else if (kode == "BBP") config->board->setTile(new FreeParkingTile(id, kode, nama, jenis), id);
            else throw ResourceException("Invalid line '" + line + "' at file " + configPath + "aksi.txt");
        }
        else if(jenis == "PAJAK"){
            if(kode == "PPH") config->board->setTile(new TaxTile(id, kode, nama, jenis, TaxTile::TAX_PPH, config->pphFlat, config->pphPercentage), id);
            else if(kode == "PBM") config->board->setTile(new TaxTile(id, kode, nama, jenis, TaxTile::TAX_PBM, config->pbmFlat, 0), id);
            else throw ResourceException("Invalid line '" + line + "' at file " + configPath + "aksi.txt");
        }
        else if(jenis == "FESTIVAL") config->board->setTile(new FestivalTile(id, kode, nama, jenis, 0, 0), id);
        else if(jenis == "KARTU") config->board->setTile(new CardTile(id, kode, nama, jenis, nullptr), id);
        else throw ResourceException("Invalid line '" + line + "' at file " + configPath + "aksi.txt");
    }


    while(std::getline(propertyFile, line)){

        if(line.empty()) continue;
        std::istringstream iss(line);

        int id; 
        std::string kode, nama, jenis, warna;
        if(!(iss >> id >> kode >> nama >> jenis >> warna)){
            throw ResourceException("Invalid line '" + line + "' at file " + configPath + "property.txt");
        }

        id--;
        if(id < 0 || id >= config->board->getTileCount()) throw ResourceException("Invalid line '" + line + "' at file " + configPath + "property.txt");

        Property *property = nullptr;
        if(jenis == "STREET"){
            int price, mortgage, upg_r, upg_h; 
            if(!(iss >> price >> mortgage >> upg_r >> upg_h)){
                throw ResourceException("Invalid line '" + line + "' at file " + configPath + "property.txt");
            }
            
            std::vector<int> rents; 
            int checkCount = 0, rent;
            while(iss >> rent){
                checkCount++;
                rents.push_back(rent);
            }
            if(checkCount < 6) throw ResourceException("Invalid line '" + line + "' at file " + configPath + "property.txt");

            property = new StreetProperty(id, kode, nama, price, mortgage, config->colorMap[warna], upg_r, upg_h, rents);
        } 

        else if(jenis == "RAILROAD"){
            int price, mortgage; 
            if(!(iss >> price >> mortgage)){
                throw ResourceException("Invalid line '" + line + "' at file " + configPath + "property.txt");
            }
            property = new RailroadProperty(id, kode, nama, price, mortgage);
        } 

        else if(jenis == "UTILITY"){
            int price, mortgage; 
            if(!(iss >> price >> mortgage)){
                throw ResourceException("Invalid line '" + line + "' at file " + configPath + "property.txt");
            }
            property = new UtilityProperty(id, kode, nama, price, mortgage);
        }

        else{
            throw ResourceException("Invalid line '" + line + "' at file " + configPath + "property.txt");
        }
        
        config->board->setTile(new PropertyTile(id, kode, nama, "PROPERTY", property), id);
    }


    return config;
}

void LoadHandler::loadSave(std::string &saveDir, GameSession* game) {
    std::string filePath = "data/" + saveDir;
    std::ifstream saveFile(filePath);
    if (!saveFile.is_open()) {
        throw ResourceException("Unable to open save file: " + filePath);
    }

    std::string line;
    
    // 1. TURN_SAAT_INI, MAX_TURN
    int currentTurn, maxTurn;
    if (!(saveFile >> currentTurn >> maxTurn)) {
        throw ResourceException("Invalid turn data in save file");
    }
    game->setCurrentTurn(currentTurn);
    game->setMaxTurn(maxTurn);

    // 2. JUMLAH_PEMAIN
    int numPlayers;
    if (!(saveFile >> numPlayers)) {
        throw ResourceException("Invalid player count in save file");
    }

    std::map<std::string, Player*> playerMap;
    std::vector<Player*> players;

    // 3. STATE_PEMAIN
    for (int i = 0; i < numPlayers; ++i) {
        std::string username, statusStr, posCode;
        int money;
        if (!(saveFile >> username >> money >> posCode >> statusStr)) {
            throw ResourceException("Invalid basic player data for " + std::to_string(i+1));
        }

        PlayerStatus status;
        if (statusStr == "ACTIVE") status = PlayerStatus::ACTIVE;
        else if (statusStr == "BANKRUPT") status = PlayerStatus::BANKRUPT;
        else if (statusStr == "JAILED") status = PlayerStatus::JAILED;
        else throw ResourceException("Invalid status " + statusStr + " for player " + username);

        Player* p = new HumanPlayer(username, i, 0); // initial money 0, will set next
        p->setMoney(money);
        p->setStatus(status);
        try {
            p->setPosition(game->getBoard()->getTileByCode(posCode)->index);
        } catch (...) {
            throw ResourceException("Invalid tile code " + posCode + " for player " + username);
        }

        int numCards;
        if (!(saveFile >> numCards)) {
            throw ResourceException("Invalid card count for player " + username);
        }
        
        std::getline(saveFile, line); // consume newline

        for (int j = 0; j < numCards; ++j) {
            if (!std::getline(saveFile, line) || line.empty()) {
                throw ResourceException("Missing card data for player " + username);
            }
            std::istringstream iss(line);
            std::string cardType;
            iss >> cardType;

            SkillCard* card = nullptr;
            if (cardType == "MoveCard") {
                int dist;
                MoveCard* mc = new MoveCard("Move_" + username + "_" + std::to_string(j));
                if (iss >> dist) mc->distance = dist;
                card = mc;
            } else if (cardType == "DiscountCard") {
                float disc; int dur;
                DiscountCard* dc = new DiscountCard("Disc_" + username + "_" + std::to_string(j));
                if (iss >> disc >> dur) { dc->discount = disc; dc->duration = dur; }
                card = dc;
            } else if (cardType == "ShieldCard") {
                int dur;
                ShieldCard* sc = new ShieldCard("Shield_" + username + "_" + std::to_string(j));
                if (iss >> dur) sc->duration = dur;
                card = sc;
            } else if (cardType == "TeleportCard") {
                card = new TeleportCard("Tele_" + username + "_" + std::to_string(j));
            } else if (cardType == "LassoCard") {
                card = new LassoCard("Lasso_" + username + "_" + std::to_string(j));
            } else if (cardType == "DemolitionCard") {
                card = new DemolitionCard("Demo_" + username + "_" + std::to_string(j));
            } else if (cardType == "FreedomCard") {
                card = new FreedomCard("Free_" + username + "_" + std::to_string(j));
            } else {
                throw ResourceException("Unknown card type: " + cardType);
            }
            if (card) p->addCard(card);
        }

        playerMap[username] = p;
        players.push_back(p);
    }

    // 4. URUTAN_GILIRAN
    std::vector<Player*> orderedPlayers;
    for (int i = 0; i < numPlayers; ++i) {
        std::string uname;
        if (!(saveFile >> uname)) throw ResourceException("Missing username in turn order");
        if (playerMap.find(uname) == playerMap.end()) throw ResourceException("Unknown username in turn order: " + uname);
        orderedPlayers.push_back(playerMap[uname]);
        orderedPlayers.back()->setPlayerIndex(i); // Update index according to new order
    }
    game->getPlayers() = orderedPlayers;

    // 5. GILIRAN_AKTIF_SAAT_INI
    std::string activeUname;
    if (!(saveFile >> activeUname)) throw ResourceException("Missing active player username");
    bool found = false;
    for (size_t i = 0; i < orderedPlayers.size(); ++i) {
        if (orderedPlayers[i]->getUsername() == activeUname) {
            game->setCurrentPlayerIndex(i);
            found = true;
            break;
        }
    }
    if (!found) throw ResourceException("Active player " + activeUname + " not found in players list");

    // 6. STATE_PROPERTI
    int numProps;
    if (!(saveFile >> numProps)) throw ResourceException("Invalid property count");
    for (int i = 0; i < numProps; ++i) {
        std::string code, type, ownerName, statusStr;
        int fMult, fDur;
        std::string nBuildStr;
        if (!(saveFile >> code >> type >> ownerName >> statusStr >> fMult >> fDur >> nBuildStr)) {
            throw ResourceException("Invalid data for property " + std::to_string(i+1));
        }

        Tile* tile = game->getBoard()->getTileByCode(code);
        auto* pt = dynamic_cast<PropertyTile*>(tile);
        if (!pt || !pt->property) throw ResourceException("Tile " + code + " is not a property");

        Property* prop = pt->property;
        prop->setFestivalMultiplier(fMult);
        prop->setFestivalDuration(fDur);

        if (ownerName == "BANK") {
            prop->setOwnerID(-1);
        } else {
            if (playerMap.find(ownerName) == playerMap.end()) throw ResourceException("Unknown owner " + ownerName + " for property " + code);
            Player* owner = playerMap[ownerName];
            prop->setOwnerID(owner->getPlayerIndex());
            owner->addProperty(prop);
        }

        if (statusStr == "BANK") prop->setStatus(PropertyStatus::BANK);
        else if (statusStr == "OWNED") prop->setStatus(PropertyStatus::OWNED);
        else if (statusStr == "MORTGAGED") prop->setStatus(PropertyStatus::MORTGAGED);
        else throw ResourceException("Invalid status " + statusStr + " for property " + code);

        if (auto* street = dynamic_cast<StreetProperty*>(prop)) {
            if (nBuildStr == "H") street->buildingCount = 5;
            else street->buildingCount = std::stoi(nBuildStr);
        }
    }

    // 7. STATE_DECK
    int numDeck;
    if (!(saveFile >> numDeck)) throw ResourceException("Invalid deck count");
    // Note: CardDeck doesn't have a clear() method. Assuming it starts empty or we just add.
    for (int i = 0; i < numDeck; ++i) {
        std::string cardType;
        if (!(saveFile >> cardType)) throw ResourceException("Missing deck card type");
        
        Card* card = nullptr;
        // Check both Action and Skill types for deck
        if (cardType == "MoveCard") card = new MoveCard("Deck_" + std::to_string(i));
        else if (cardType == "DiscountCard") card = new DiscountCard("Deck_" + std::to_string(i));
        else if (cardType == "ShieldCard") card = new ShieldCard("Deck_" + std::to_string(i));
        else if (cardType == "TeleportCard") card = new TeleportCard("Deck_" + std::to_string(i));
        else if (cardType == "LassoCard") card = new LassoCard("Deck_" + std::to_string(i));
        else if (cardType == "DemolitionCard") card = new DemolitionCard("Deck_" + std::to_string(i));
        else if (cardType == "FreedomCard") card = new FreedomCard("Deck_" + std::to_string(i));
        else if (cardType == "GoToStationCard") card = new GoToStationCard("Deck_" + std::to_string(i));
        else if (cardType == "MoveBackCard") card = new MoveBackCard("Deck_" + std::to_string(i), 3); // Default 3
        else if (cardType == "GoToJailCard") card = new GoToJailCard("Deck_" + std::to_string(i));
        else if (cardType == "GratificationCard") card = new GratificationCard("Deck_" + std::to_string(i), 100);
        else if (cardType == "BirthdayGiftCard") card = new BirthdayGiftCard("Deck_" + std::to_string(i), 50);
        else if (cardType == "DoctorFeeCard") card = new DoctorFeeCard("Deck_" + std::to_string(i), 50);
        else throw ResourceException("Unknown deck card type: " + cardType);

        if (card) game->getDeck().addCard(card);
    }

    // 8. STATE_LOG
    int numLogs;
    if (!(saveFile >> numLogs)) throw ResourceException("Invalid log count");
    std::getline(saveFile, line); // consume newline
    for (int i = 0; i < numLogs; ++i) {
        if (!std::getline(saveFile, line) || line.empty()) break;
        std::istringstream iss(line);
        int logTurn; std::string logUser, logAction, logDetail;
        iss >> logTurn >> logUser >> logAction;
        std::getline(iss, logDetail);
        if (!logDetail.empty() && logDetail[0] == ' ') logDetail.erase(0, 1);
        game->getLogger()->log(logTurn, logUser, logAction, logDetail);
    }
}
