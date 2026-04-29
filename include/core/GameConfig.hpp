#pragma once

#include <string>
#include <unordered_map>
#include "Color.hpp"
#include "Board.hpp"

class GameConfig {
public:
    int maxTurn, initialBalance;
    int pphFlat, pphPercentage, pbmFlat;
    int goSalary, jailFine;

    Board *board;
    std::map<int, int> railroadRentTable, utilityMultiplierTable;
    std::unordered_map<std::string, Color> colorMap;


    GameConfig() : board(new Board(40)), colorMap({
            {"DEFAULT", Color::DEFAULT},
            {"PUTIH", Color::PUTIH},
            {"HITAM", Color::HITAM},
            {"MERAH", Color::MERAH},
            {"HIJAU", Color::HIJAU},
            {"BIRU", Color::BIRU},
            {"KUNING", Color::KUNING},
            {"COKLAT", Color::COKLAT},
            {"ORANYE", Color::ORANYE},
            {"JINGGA", Color::JINGGA},
            {"MERAH_MUDA", Color::MERAH_MUDA},
            {"UNGU", Color::UNGU},
            {"ABU_ABU", Color::ABU_ABU},
            {"EMAS", Color::EMAS},
            {"MAROON", Color::MAROON},
            {"MAGENTA", Color::MAGENTA},
            {"TRANSPARAN", Color::TRANSPARAN}
        })
    {}


    ~GameConfig() = default;
};