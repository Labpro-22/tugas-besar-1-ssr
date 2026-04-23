#pragma once

#include <string>
#include <vector>
#include <map>

#include "Tile.hpp"
#include "AppException.hpp"
#include <raylib-cpp.hpp>

class Tile;

class Board {
private:
    std::vector<Tile*> tiles;
    std::map<std::string, Tile*> tileMap;
    int totalTiles;

public:
    Board(int totalTiles);
    ~Board();

    int getTileCount();
    Tile  *getTile(int index);
    Tile *getTileByCode(std::string code);
    std::vector<int> getTilesByColor(raylib::Color color);
    int getNextTileIndex(int current, int steps);
    int findNearestStation(int fromIndex);
    int getJailIndex();
    int getStartTileIndex();
    void setTile(Tile* tile, int idx);
};
