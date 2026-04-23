#include "Property.hpp"
#include <stdexcept>
#include <iomanip>

UtilityProperty::UtilityProperty(int ID, string code, string name, int price, int mortgageValue, raylib::Color color)
    : Property(ID, code, name, "UTILITY", price, mortgageValue, color) {}
 
UtilityProperty::UtilityProperty(int ID, string code, string name, int price, int mortgageValue)
    : Property(ID, code, name, "UTILITY", price, mortgageValue, raylib::Color::White()) {}

UtilityProperty::~UtilityProperty() {}
 
int UtilityProperty::calculateRent(int diceNum, int ownedCountOfType, bool isMono) {
    return diceNum * ownedCountOfType *festivalMultiplier;
}