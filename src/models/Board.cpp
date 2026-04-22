#include "Board.hpp"

#include "Property.hpp"

Board::Board(int totalTiles)
    : tiles(totalTiles > 0 ? (totalTiles) : 0),
      totalTiles(totalTiles) {}

Board::~Board() {
    for (Tile* tile : tiles) {
        delete tile;
    }
}

int Board::getTileCount() {
    return totalTiles;
}

Tile* Board::getTile(int index) {
    if (index < 0 || index >= totalTiles) {
        throw GameException("Board", "Tile with index {" + std::to_string(index) + "} out of bound");
    }
    return tiles[index];
}

Tile* Board::getTileByCode(std::string code) {
    auto it = tileMap.find(code);
    if (it == tileMap.end()) {
        throw GameException("Board", "Tile with code {" + code + "} not found");
    }
    return it->second;
}

int Board::getNextTileIndex(int current, int steps) {
    if (totalTiles <= 0) {
        throw GameException("Board", "Next tile's index is out of bound");
    }

    const int normalizedCurrent = ((current % totalTiles) + totalTiles) % totalTiles;
    const int raw = normalizedCurrent + steps;
    return ((raw % totalTiles) + totalTiles) % totalTiles;
}

int Board::findNearestStation(int fromIndex) {
    if (totalTiles <= 0) {
        throw GameException("Board", "Cannot find nearest station on an empty board");
    }

    const int normalizedCurrent = ((fromIndex % totalTiles) + totalTiles) % totalTiles;
    int bestIndex = -1;
    int bestDistance = totalTiles + 1;

    for (Tile* tile : tiles) {
        auto* propertyTile = dynamic_cast<PropertyTile*>(tile);
        if (propertyTile == nullptr || propertyTile->property == nullptr) {
            continue;
        }
        if (propertyTile->property->getType() != "RAILROAD") {
            continue;
        }

        const int distance = ((tile->index - normalizedCurrent) + totalTiles) % totalTiles;
        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = tile->index;
        }
    }

    if (bestIndex == -1) {
        throw GameException("Board", "No railroad tile found");
    }

    return bestIndex;
}

int Board::getJailIndex() {
    for(Tile* tile: tiles){
        if (dynamic_cast<const JailTile*>(tile) != nullptr){
            return tile->index;
        }
    }
    throw GameException("Board", "Jail tile's index not found");
}

int Board::getStartTileIndex() {
    if(totalTiles > 0){
        return 0;
    }else{
        throw GameException("Board", "Total tiles is less than or equal to zero");
    }
}

void Board::setTile(Tile* tile, int idx) {
    if (tile == nullptr) {
        throw GameException("Board", "Tile is empty(nullptr)");
    }
    if(idx < 0 || idx >= totalTiles) {
        throw GameException("Board", "Tile with index {" + std::to_string(idx) + "} is out of bounds");
    }

    Tile* oldTile = tiles[idx];
    if (oldTile != nullptr) {
        tileMap.erase(oldTile->code);
        delete oldTile;
    }

    tile->index = idx;
    tiles[idx] = tile;
    tileMap[tile->code] = tile;
}

std::vector<int> Board::getTilesByColor(raylib::Color color) {
    std::vector<int> result;
    for (Tile* tile : tiles) {
        auto* propertyTile = dynamic_cast<PropertyTile*>(tile);
        if (propertyTile == nullptr || propertyTile->property == nullptr) {
            continue;
        }

        auto* street = dynamic_cast<StreetProperty*>(propertyTile->property);
        if (street != nullptr && street->colorGroup == color) {
            result.push_back(tile->index);
        }
    }
    return result;
}

