#include "Property.hpp"
#include "GameApp.hpp"

#include <stdexcept>
#include <iomanip>

UtilityProperty::UtilityProperty(int ID, string code, string name, int price, int mortgageValue, Color color)
    : Property(ID, code, name, "UTILITY", price, mortgageValue, color) {}
 
UtilityProperty::UtilityProperty(int ID, string code, string name, int price, int mortgageValue)
    : Property(ID, code, name, "UTILITY", price, mortgageValue, Color::PUTIH) {}

UtilityProperty::~UtilityProperty() {}
 
int UtilityProperty::calculateRent(int diceNum, int ownedCountOfType, bool isMono) {
    (void) isMono;
    GameSession *game = GameApp::currentSession;
    auto &multiplierTable = game->getConfig()->utilityMultiplierTable;

    while(multiplierTable.find(ownedCountOfType) == multiplierTable.end()){
        ownedCountOfType--;
    }

    return diceNum * multiplierTable[ownedCountOfType] * festivalMultiplier;
}