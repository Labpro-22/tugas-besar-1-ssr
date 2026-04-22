#pragma once

#include <string>
#include <vector>
#include <map>

#include "Tile.hpp"

class Board {
private:
    std::vector<Tile*> tiles;
    std::map<std::string, Tile*> tileMap;
    int totalTiles;

public:
    Board(int totalTiles) : totalTiles(totalTiles) {}
    ~Board();

    int getTileCount();
    Tile  *getTile(int index);
    Tile *getTileByCode(std::string code);
    int getNextTileIndex(int current, int steps);
    int findNearestStation(int fromIndex);
    int getJailIndex();
    int getStartTileIndex();
    void setTile(Tile* tile, int idx);
};
