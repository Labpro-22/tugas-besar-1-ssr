#pragma once

#include <string>
#include <vector>

#include "GameConfig.hpp"

class GameSession;
class GameConfig;

class LoadHandler {
public:
    LoadHandler();
    ~LoadHandler() = default;

    GameConfig *loadConfiguration(std::string &configDir);
    void loadSave(std::string &saveDataDir, GameSession *game);
};

class SaveHandler {
private:
    std::string savePath;

public:
    SaveHandler(const std::string& path);
    ~SaveHandler() = default;

    void save(GameSession* game);
};
