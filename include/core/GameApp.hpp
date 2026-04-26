#pragma once

#include <string>

#include "GameSession.hpp"

class GameApp {
    std::string gameTitle;
    std::string gameDescription;

    void printStartArt();
    void printEndArt();

public:
    static GameSession *currentSession;

    GameApp(std::string title, std::string desc) : gameTitle(title), gameDescription(desc) {}
    ~GameApp();

    void start();
};