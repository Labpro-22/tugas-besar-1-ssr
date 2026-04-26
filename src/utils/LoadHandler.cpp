#include "SaveLoadHandler.hpp"
#include "GameSession.hpp"
#include "Board.hpp"
#include "Property.hpp"
#include "Tile.hpp"
#include "AppException.hpp"
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

void LoadHandler::loadSave(std::string &saveDir, GameSession* game) {}
