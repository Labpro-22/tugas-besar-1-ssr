#pragma once

#include <string>
#include <unordered_map>

#include "Board.hpp"

class GameConfig {
public:
    int maxTurn, initialBalance;
    int pphFlat, pphPercentage, pbmFlat;
    int goSalary, jailFine;

    Board *board;
    std::map<int, int> railroadRentTable, utilityMultiplierTable;
    std::unordered_map<std::string, raylib::Color> colorMap;


    GameConfig() : board(new Board(40)), colorMap({
            {"DEFAULT", raylib::Color::Black()},
            {"PUTIH", raylib::Color::White()},
            {"HITAM", raylib::Color::Black()},
            {"MERAH", raylib::Color::Red()},
            {"HIJAU", raylib::Color::Lime()},
            {"BIRU", raylib::Color::Blue()},
            {"KUNING",raylib::Color::Yellow()},
            {"COKLAT",raylib::Color::Brown()},
            {"ORANYE",raylib::Color::Orange()},
            {"JINGGA",raylib::Color::Orange()},
            {"MERAH_MUDA", raylib::Color::Pink()},
            {"UNGU", raylib::Color::Purple()},
            {"ABU_ABU", raylib::Color::Gray()},
            {"EMAS", raylib::Color::Gold()},
            {"MAROON", raylib::Color::Maroon()},
            {"MAGENTA", raylib::Color::Magenta()},
            {"TRANSPARAN", raylib::Color::Blank()}
        })
    {}


    ~GameConfig() = default;
};