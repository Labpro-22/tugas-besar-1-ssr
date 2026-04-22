#include "Property.hpp"
#include <stdexcept>
#include <iomanip>

UtilityProperty::UtilityProperty(int ID, string code, string name,
                                  int price, int mortgageValue)
    : Property(ID, code, name, "UTILITY", price, mortgageValue) {}
 
UtilityProperty::~UtilityProperty() {}
 
int UtilityProperty::calculateRent(int diceNum, int ownedCountOfType) {
    return diceNum * ownedCountOfType;
}