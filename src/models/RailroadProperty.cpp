#include "Property.hpp"
#include "GameApp.hpp"
#include "Color.hpp"

#include <stdexcept>
#include <iomanip>

RailroadProperty::RailroadProperty(int ID, string code, string name, int price, int mortgageValue, Color color)
    : Property(ID, code, name, "RAILROAD", price, mortgageValue, color) {}
RailroadProperty::RailroadProperty(int ID, string code, string name, int price, int mortgageValue)
    : Property(ID, code, name, "RAILROAD", price, mortgageValue, Color::PUTIH) {}
RailroadProperty::~RailroadProperty() {}
 
int RailroadProperty::calculateRent(int diceNum, int ownedCount, bool isMono) {
    (void) isMono;

    GameSession *game = GameApp::currentSession;
    auto &rentTable = game->getConfig()->railroadRentTable;

    while(rentTable.find(ownedCount) == rentTable.end()){
        ownedCount--;
    }
    
    return rentTable[ownedCount] * festivalMultiplier;
}