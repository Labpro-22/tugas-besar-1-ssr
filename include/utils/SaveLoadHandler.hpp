#pragma once

#include <string>

class Game;

class SaveHandler {
private:
    std::string savePath;

public:
    SaveHandler(const std::string& path);
    void save(Game* game);
};

class LoadHandler {
private:
    std::string loadPath;

public:
    LoadHandler(const std::string& path);
    Game* load();
};