#include "Board.hpp"
#include "Property.hpp"
#include "Tile.hpp"
#include "AppException.hpp"
#include "GameApp.hpp"
#include "GameSession.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <utility>

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
    return 9; // Assume default is index 10 (9 if 0-indexed)
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

    auto colorBrown = raylib::Color{139, 90, 43, 255};
    auto colorLightBlue = raylib::Color{0, 191, 255, 255};
    auto colorPink = raylib::Color{255, 105, 180, 255};
    auto colorOrange = raylib::Color{255, 165, 0, 255};
    auto colorRed = raylib::Color{220, 20, 60, 255};
    auto colorYellow = raylib::Color{255, 215, 0, 255};
    auto colorGreen = raylib::Color{34, 139, 34, 255};
    auto colorDarkBlue = raylib::Color{0, 0, 139, 255};
    auto colorGray = raylib::Color{130, 130, 130, 255};

    setTile(new StartTile(0, "GO", "Petak Mulai", "SPESIAL", 200), 0);
    setTile(new PropertyTile(1, "GRT", "Garut", "PROPERTY", new StreetProperty(1, "GRT", "GARUT", 60, 40, colorBrown, 20, 50, {2, 10, 30, 90, 160, 250})), 1);
    setTile(new CardTile(2, "DNU", "Dana Umum", "KARTU", nullptr), 2);
    setTile(new PropertyTile(3, "TSK", "Tasikmalaya", "PROPERTY", new StreetProperty(3, "TSK", "TASIKMALAYA", 60, 40, colorBrown, 50, 50, {4, 20, 60, 180, 320, 450})), 3);
    setTile(new TaxTile(4, "PPH", "Pajak Penghasilan", "PAJAK", TaxTile::TAX_PPH, 150, 10.0), 4);
    setTile(new PropertyTile(5, "GBR", "Stasiun Gambir", "PROPERTY", new RailroadProperty(5, "GBR", "STASIUN_GAMBIR", 200, 100)), 5);
    setTile(new PropertyTile(6, "BGR", "Bogor", "PROPERTY", new StreetProperty(6, "BGR", "BOGOR", 100, 80, colorLightBlue, 20, 50, {6, 30, 90, 270, 400, 550})), 6);
    setTile(new FestivalTile(7, "FES", "Festival", "FESTIVAL", 0.0, 0), 7);
    setTile(new PropertyTile(8, "DPK", "Depok", "PROPERTY", new StreetProperty(8, "DPK", "DEPOK", 100, 80, colorLightBlue, 20, 50, {6, 30, 90, 270, 400, 550})), 8);
    setTile(new PropertyTile(9, "BKS", "Bekasi", "PROPERTY", new StreetProperty(9, "BKS", "BEKASI", 120, 90, colorLightBlue, 20, 50, {8, 40, 100, 300, 450, 600})), 9);
    setTile(new JailTile(10, "PEN", "Penjara", "SPESIAL"), 10);
    setTile(new PropertyTile(11, "MGL", "Magelang", "PROPERTY", new StreetProperty(11, "MGL", "MAGELANG", 140, 100, colorPink, 100, 100, {10, 50, 150, 450, 625, 750})), 11);
    setTile(new PropertyTile(12, "PLN", "PLN", "PROPERTY", new UtilityProperty(12, "PLN", "PLN", 150, 75, colorGray)), 12);
    setTile(new PropertyTile(13, "SOL", "Solo", "PROPERTY", new StreetProperty(13, "SOL", "SOLO", 140, 100, colorPink, 100, 100, {10, 50, 150, 450, 625, 750})), 13);
    setTile(new PropertyTile(14, "YOG", "Yogyakarta", "PROPERTY", new StreetProperty(14, "YOG", "YOGYAKARTA", 160, 120, colorPink, 100, 100, {12, 60, 180, 500, 700, 900})), 14);
    setTile(new PropertyTile(15, "STB", "Stasiun Bandung", "PROPERTY", new RailroadProperty(15, "STB", "STASIUN_BANDUNG", 200, 100)), 15);
    setTile(new PropertyTile(16, "MAL", "Malang", "PROPERTY", new StreetProperty(16, "MAL", "MALANG", 180, 135, colorOrange, 100, 100, {14, 70, 200, 550, 750, 950})), 16);
    setTile(new CardTile(17, "DNU", "Dana Umum", "KARTU", nullptr), 17);
    setTile(new PropertyTile(18, "SMG", "Semarang", "PROPERTY", new StreetProperty(18, "SMG", "SEMARANG", 180, 140, colorOrange, 100, 100, {14, 70, 200, 550, 750, 950})), 18);
    setTile(new PropertyTile(19, "SBY", "Surabaya", "PROPERTY", new StreetProperty(19, "SBY", "SURABAYA", 200, 150, colorOrange, 100, 100, {16, 80, 220, 600, 800, 1000})), 19);
    setTile(new FreeParkingTile(20, "BBP", "Bebas Parkir", "SPESIAL"), 20);
    setTile(new PropertyTile(21, "MKS", "Makassar", "PROPERTY", new StreetProperty(21, "MKS", "MAKASSAR", 220, 175, colorRed, 150, 150, {18, 90, 250, 700, 875, 1050})), 21);
    setTile(new CardTile(22, "KSP", "Kesempatan", "KARTU", nullptr), 22);
    setTile(new PropertyTile(23, "BLP", "Balikpapan", "PROPERTY", new StreetProperty(23, "BLP", "BALIKPAPAN", 220, 175, colorRed, 150, 150, {18, 90, 250, 700, 875, 1050})), 23);
    setTile(new PropertyTile(24, "MND", "Manado", "PROPERTY", new StreetProperty(24, "MND", "MANADO", 240, 190, colorRed, 150, 150, {20, 100, 300, 750, 925, 1100})), 24);
    setTile(new PropertyTile(25, "TUG", "Stasiun Tugu", "PROPERTY", new RailroadProperty(25, "TUG", "STASIUN_TUGU", 200, 100)), 25);
    setTile(new PropertyTile(26, "PLB", "Palembang", "PROPERTY", new StreetProperty(26, "PLB", "PALEMBANG", 260, 200, colorYellow, 150, 150, {22, 110, 330, 800, 975, 1150})), 26);
    setTile(new PropertyTile(27, "PKB", "Pekanbaru", "PROPERTY", new StreetProperty(27, "PKB", "PEKANBARU", 260, 210, colorYellow, 150, 150, {22, 110, 330, 800, 975, 1150})), 27);
    setTile(new PropertyTile(28, "PAM", "PAM", "PROPERTY", new UtilityProperty(28, "PAM", "PAM", 150, 75, colorGray)), 28);
    setTile(new PropertyTile(29, "MED", "Medan", "PROPERTY", new StreetProperty(29, "MED", "MEDAN", 280, 225, colorYellow, 150, 150, {24, 120, 360, 850, 1025, 1200})), 29);
    setTile(new GoToJailTile(30, "PPJ", "Petak Pergi ke Penjara", "SPESIAL"), 30);
    setTile(new PropertyTile(31, "BDG", "Bandung", "PROPERTY", new StreetProperty(31, "BDG", "BANDUNG", 300, 250, colorGreen, 200, 200, {26, 130, 390, 900, 1100, 1275})), 31);
    setTile(new PropertyTile(32, "DEN", "Denapasar", "PROPERTY", new StreetProperty(32, "DEN", "DENPASAR", 300, 260, colorGreen, 200, 200, {26, 130, 390, 900, 1100, 1275})), 32);
    setTile(new FestivalTile(33, "FES", "Festival", "FESTIVAL", 0.0, 0), 33);
    setTile(new PropertyTile(34, "MTR", "Mataram", "PROPERTY", new StreetProperty(34, "MTR", "MATARAM", 320, 280, colorGreen, 200, 200, {28, 150, 450, 1000, 1200, 1400})), 34);
    setTile(new PropertyTile(35, "GUB", "Stasiun Gubeng", "PROPERTY", new RailroadProperty(35, "GUB", "STASIUN_GUBENG", 200, 100)), 35);
    setTile(new CardTile(36, "KSP", "Kesempatan", "KARTU", nullptr), 36);
    setTile(new PropertyTile(37, "JKT", "Jakarta", "PROPERTY", new StreetProperty(37, "JKT", "JAKARTA", 350, 300, colorDarkBlue, 200, 200, {35, 175, 500, 1100, 1300, 1500})), 37);
    setTile(new TaxTile(38, "PBM", "Pajak Barang Mewah", "PAJAK", TaxTile::TAX_PBM, 200, 0.0), 38);
    setTile(new PropertyTile(39, "IKN", "Ibu Kota Nusantara", "PROPERTY", new StreetProperty(39, "IKN", "IBU_KOTA_NUSANTARA", 400, 350, colorDarkBlue, 200, 200, {50, 200, 600, 1400, 1700, 2000})), 39);
}

void Board::printBoard() {
    GameSession* session = GameApp::currentSession;

    // 1. Color Mapping & Translation Table Helpers
    auto getColorInfo = [](raylib::Color c) -> std::pair<std::string, std::string> {
        if (c.r == 139 && c.g == 90 && c.b == 43) return {"C", "COKLAT"};
        if (c.r == 0 && c.g == 191 && c.b == 255) return {"B", "BIRU_MUDA"};
        if (c.r == 255 && c.g == 105 && c.b == 180) return {"P", "PINK"};
        if (c.r == 255 && c.g == 165 && c.b == 0) return {"O", "ORANYE"};
        if (c.r == 220 && c.g == 20 && c.b == 60) return {"M", "MERAH"};
        if (c.r == 255 && c.g == 215 && c.b == 0) return {"K", "KUNING"};
        if (c.r == 34 && c.g == 139 && c.b == 34) return {"H", "HIJAU"};
        if (c.r == 0 && c.g == 0 && c.b == 139) return {"T", "BIRU_TUA"};
        if (c.r == 130 && c.g == 130 && c.b == 130) return {"A", "ABU_ABU"};
        if (c.r == 230 && c.g == 41 && c.b == 55) return {"M", "MERAH"};
        if (c.r == 0 && c.g == 228 && c.b == 48) return {"H", "HIJAU"};
        if (c.r == 0 && c.g == 121 && c.b == 241) return {"B", "BIRU"};
        if (c.r == 253 && c.g == 249 && c.b == 0) return {"K", "KUNING"};
        if (c.r == 127 && c.g == 106 && c.b == 79) return {"C", "COKLAT"};
        if (c.r == 255 && c.g == 161 && c.b == 0) return {"O", "ORANYE"};
        if (c.r == 255 && c.g == 109 && c.b == 194) return {"P", "MERAH_MUDA"};
        if (c.r == 200 && c.g == 122 && c.b == 255) return {"U", "UNGU"};
        if (c.r == 130 && c.g == 130 && c.b == 130) return {"A", "ABU_ABU"};
        if (c.r == 255 && c.g == 203 && c.b == 0) return {"E", "EMAS"};
        if (c.r == 190 && c.g == 33 && c.b == 55) return {"R", "MAROON"};
        if (c.r == 255 && c.g == 0 && c.b == 255) return {"G", "MAGENTA"};
        if (c.r == 255 && c.g == 255 && c.b == 255) return {"W", "PUTIH"};
        if (c.r == 0 && c.g == 0 && c.b == 0) return {"T", "HITAM"};
        return {" ", ""};
    };

    auto getAnsiColor = [](raylib::Color c) {
        if (c.a == 0) return std::string("\033[0m"); 
        return "\033[38;2;" + std::to_string(c.r) + ";" + std::to_string(c.g) + ";" + std::to_string(c.b) + "m";
    };

    std::map<std::string, std::string> usedColors;
    for (int i = 0; i < totalTiles; ++i) {
        if (!tiles[i]) continue;
        auto* pt = dynamic_cast<PropertyTile*>(tiles[i]);
        if (pt && pt->property) {
            auto info = getColorInfo(pt->property->getColorGroup());
            if (!info.second.empty()) usedColors[info.first] = info.second;
        }
    }

    // 2. Middle Information (Top)
    std::cout << "=========================================================================================\n";
    std::cout << "                                     NIMONSPOLI BOARD                                    \n";
    std::cout << "=========================================================================================\n";
    
    if (session) {
        std::cout << " Players:\n";
        for (Player* p : session->getPlayers()) {
            if (!p) continue;
            std::string posName = "Unknown";
            if (p->getPosition() >= 0 && p->getPosition() < totalTiles && tiles[p->getPosition()]) {
                posName = tiles[p->getPosition()]->getName();
            }
            std::cout << "  [" << (p->getPlayerIndex() + 1) << "] " 
                      << std::left << std::setw(15) << p->getUsername() 
                      << " : M" << std::setw(8) << p->getMoney() 
                      << " @ " << posName << "\n";
        }
    }

    if (!usedColors.empty()) {
        std::cout << " Color Codes:\n ";
        int count = 0;
        for (auto const& [code, name] : usedColors) {
            std::cout << "[" << code << "]: " << std::left << std::setw(12) << name << "  ";
            if (++count % 4 == 0) std::cout << "\n ";
        }
        if (count % 4 != 0) std::cout << "\n";
    }
    std::cout << "=========================================================================================\n\n";

    // 3. Helpers
    auto center = [](std::string s, int width) {
        if (s.length() >= (size_t)width) return s.substr(0, width);
        int left = (width - s.length()) / 2;
        int right = width - s.length() - left;
        return std::string(left, ' ') + s + std::string(right, ' ');
    };

    // 4. Prepare Grid Data
    std::vector<std::vector<std::string>> grid(40, std::vector<std::string>(3, "       "));
    for (int i = 0; i < 40; ++i) {
        if (i >= totalTiles || !tiles[i]) continue;
        Tile* t = tiles[i];

        // Line 0: Code + Color Letter
        std::string code = t->code;
        std::string colorLetter = "";
        std::string ansi = "";
        auto* pt = dynamic_cast<PropertyTile*>(t);
        if (pt && pt->property) {
            auto info = getColorInfo(pt->property->getColorGroup());
            colorLetter = info.first;
            ansi = getAnsiColor(pt->property->getColorGroup());
        }

        std::string displayCode = code;
        if (!colorLetter.empty() && colorLetter != " ") {
            displayCode = colorLetter + "-" + code;
        }
        grid[i][0] = ansi + center(displayCode, 7) + "\033[0m";

        // Line 1: Owner and Buildings
        std::string ownerStr = "";
        if (pt && pt->property && pt->property->getOwnerID() != -1) {
            ownerStr = "P" + std::to_string(pt->property->getOwnerID() + 1);
        }
        
        std::string buildStr = "";
        if (pt && pt->property) {
            auto* sp = dynamic_cast<StreetProperty*>(pt->property);
            if (sp && sp->getBuildingCount() > 0) {
                if (sp->getBuildingCount() == 5) buildStr = "HOT";
                else buildStr = "H" + std::to_string(sp->getBuildingCount());
            }
        }
        
        std::string line1 = ownerStr;
        if (!buildStr.empty()) {
            if (!line1.empty()) line1 += " ";
            line1 += buildStr;
        }
        grid[i][1] = center(line1, 7);

        // Line 2: Players
        std::string playerIndicesStr = "";
        if (session) {
            for (Player* p : session->getPlayers()) {
                if (p && p->getPosition() == i) {
                    playerIndicesStr += std::to_string(p->getPlayerIndex() + 1);
                }
            }
        }
        if (!playerIndicesStr.empty()) grid[i][2] = center("*" + playerIndicesStr + "*", 7);
        else grid[i][2] = "       ";
    }

    // 5. Printing
    auto printBorder = [&](const std::vector<int>& indices) {
        for (int idx : indices) std::cout << "+-------";
        std::cout << "+\n";
    };

    // Top Row (20-30)
    std::vector<int> topRow = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    printBorder(topRow);
    printBorder(topRow);
    for (int l = 0; l < 3; ++l) {
        for (int idx : topRow) std::cout << "|" << grid[idx][l];
        std::cout << "|\n";
    }
    printBorder(topRow);

    // Sides (19-11 left, 31-39 right)
    for (int i = 0; i < 9; ++i) {
        int left = 19 - i;
        int right = 31 + i;
        for (int l = 0; l < 3; ++l) {
            std::cout << "|" << grid[left][l] << "|";
            std::cout << std::string(71, ' ');
            std::cout << "|" << grid[right][l] << "|\n";
        }
        // Border for side tiles
        std::cout << "+-------+";
        std::cout << std::string(71, ' ');
        std::cout << "+-------+\n";
    }

    // Bottom Row (10-0)
    std::vector<int> bottomRow = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    printBorder(bottomRow);
    for (int l = 0; l < 3; ++l) {
        for (int idx : bottomRow) std::cout << "|" << grid[idx][l];
        std::cout << "|\n";
    }
    printBorder(bottomRow);
    printBorder(bottomRow);
}
