#pragma once

#include <string>
#include <vector>

#include "Game.hpp"
#include "Player.hpp"
#include "Property.hpp"
#include "TransactionLogger.hpp"


class LoadHandler {
private:
    std::string loadPath;

    std::vector<Property>* loadPropertyOverview(std::string filePath);
    void loadRailroadRent(std::string filePath);
    void loadUtilityRent(std::string filePath);
    void loadTaxConfig(std::string filePath);
    void loadSpecialConfig(std::string filePath);
    void loadMiscConfig(std::string filePath);
    void loadBoardConfig(std::string filePath);

public:
    LoadHandler(std::string path);
    ~LoadHandler() = default;
    Game* load();
};



class SaveHandler {
private:
    std::string savePath;

    void savePlayerState(Player* player);
    void savePropertyState(Property* prop);
    void saveDeckState(Game* game);
    void saveLogState(TransactionLogger* logger);

public:
    SaveHandler(std::string path);
    ~SaveHandler() = default;

    void save(Game* game);
};