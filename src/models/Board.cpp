#include "Board.hpp"
#include "Property.hpp"
#include "Tile.hpp"
#include "AppException.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

Board::Board(int totalTiles)
    : tiles(totalTiles > 0 ? (totalTiles) : 0), totalTiles(totalTiles) { tiles.assign(totalTiles, nullptr); }

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
        if (tile && dynamic_cast<const JailTile*>(tile) != nullptr){
            return tile->index;
        }
    }
    // Fallback to index 10 if not found
    return 10 % totalTiles;
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
        if (!tile) continue;
        auto* propertyTile = dynamic_cast<PropertyTile*>(tile);
        if (propertyTile == nullptr || propertyTile->property == nullptr) {
            continue;
        }

        auto* street = dynamic_cast<StreetProperty*>(propertyTile->property);
        if (street != nullptr && street->getColorGroup().r == color.r && street->getColorGroup().g == color.g && street->getColorGroup().b == color.b) {
            result.push_back(tile->index);
        }
    }
    return result;
}


void Board::initializeDefault(){
    for (int i = 0; i < totalTiles; ++i) {
        setTile(new FreeParkingTile(i, "PARK", "Bebas Parkir", "SPESIAL"), i); // Placeholder
    }
    setTile(new StartTile(0, "GO", "Petak Mulai", "SPESIAL", 200), 0);
}

void Board::printBoard(){
    std::cout << "--- PAPAN PERMAINAN NIMONSPOLI ---\n";
    for (int i = 0; i < totalTiles; ++i) {
        if (!tiles[i]) continue;
        
        std::cout << std::setw(2) << i+1 << ". [" << tiles[i]->code << "] " << std::left << std::setw(20) << tiles[i]->name;
        
        auto* propTile = dynamic_cast<PropertyTile*>(tiles[i]);
        if (propTile && propTile->property) {
            Property* p = propTile->property;
            std::cout << " | Status: ";
            switch(p->getStatus()) {
                case PropertyStatus::BANK: std::cout << "BANK"; break;
                case PropertyStatus::OWNED: std::cout << "OWNER: " << p->getOwnerID(); break;
                case PropertyStatus::MORTGAGED: std::cout << "MORTGAGED"; break;
            }
            if (auto* sp = dynamic_cast<StreetProperty*>(p)) {
                if (sp->getBuildingCount() > 0) {
                    if (sp->getBuildingCount() == 5) std::cout << " (HOTEL)";
                    else std::cout << " (" << sp->getBuildingCount() << " RUMAH)";
                }
            }
        }
        std::cout << "\n";
    }
}
