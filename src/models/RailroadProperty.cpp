#include "Property.hpp"
#include <stdexcept>
#include <iomanip>

std::vector<int> RailroadProperty::rentTable = {0, 25, 50, 100, 200};

RailroadProperty::RailroadProperty(int ID, string code, string name, int price, int mortgageValue, raylib::Color color)
    : Property(ID, code, name, "RAILROAD", price, mortgageValue, color) {}
RailroadProperty::RailroadProperty(int ID, string code, string name, int price, int mortgageValue)
    : Property(ID, code, name, "RAILROAD", price, mortgageValue, raylib::Color::White()) {}
RailroadProperty::~RailroadProperty() {}
 
int RailroadProperty::calculateRent(int diceNum, int ownedCount, bool isMono) {
    int count = min((int)ownedCount, (int)rentTable.size() - 1);
    return rentTable[count] * festivalMultiplier;
}