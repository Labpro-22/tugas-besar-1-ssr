#include "Property.hpp"
#include <stdexcept>
#include <iomanip>

std::vector<int> UtilityProperty::multiplierTable = {0, 4, 10};

UtilityProperty::UtilityProperty(int ID, string code, string name, int price, int mortgageValue, raylib::Color color)
    : Property(ID, code, name, "UTILITY", price, mortgageValue, color) {}
 
UtilityProperty::UtilityProperty(int ID, string code, string name, int price, int mortgageValue)
    : Property(ID, code, name, "UTILITY", price, mortgageValue, raylib::Color::White()) {}

UtilityProperty::~UtilityProperty() {}
 
int UtilityProperty::calculateRent(int diceNum, int ownedCountOfType, bool isMono) {
    int count = min((int)ownedCountOfType, (int)multiplierTable.size() - 1);
    return diceNum * multiplierTable[count] * festivalMultiplier;
}