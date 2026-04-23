#include "Property.hpp"
#include <stdexcept>
#include <iomanip>

RailroadProperty::RailroadProperty(int ID, string code, string name, int price, int mortgageValue, raylib::Color color)
    : Property(ID, code, name, "RAILROAD", price, mortgageValue, color) {}
RailroadProperty::~RailroadProperty() {}
 
int RailroadProperty::calculateRent(int diceNum, int ownedCount, bool isMono) {
    return ownedCount*festivalMultiplier;
}