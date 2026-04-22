#include "Property.hpp"
#include <stdexcept>
#include <iomanip>

StreetProperty::StreetProperty(int ID, string code, string name,
    int price, int mortgageValue, raylib::Color color, int housePrice, int hotelPrice, vector<int> rentLevels)
    : Property(ID, code, name, "STREET", price, mortgageValue),
    colorGroup(color),
    housePrice(housePrice),
    hotelPrice(hotelPrice),
    rentLevels(rentLevels),
    buildingCount(0),
    festivalMultiplier(1),
    festivalDuration(0) {}
 
StreetProperty::~StreetProperty() {}

int StreetProperty::mortgage() override{
    if(status != PropertyStatus::OWNED){
        throw runtime_error("Properti " + code + " tidak dapat digadaikan (status bukan OWNED).");
    }if(buildingCount > 0){
        sellBuilding
    }
    status = PropertyStatus::MORTGAGED;
    return mortgageValue;
}

int calculateRent(int diceNum, int ownedCountOfType) override{
    if(this->isMonopolized()){
        
    }
}

int StreetProperty::calculateRent(int diceNum, int ownedCountOfType) {
    int baseRent = 0;
    if (buildingCount == 0) {
        baseRent = rentLevels[0];
        if (ownedCountOfType > 0) { // caller passes non-zero when monopolised
            baseRent *= 2;
        }
    } else if (buildingCount <= 5) {
        baseRent = rentLevels[buildingCount];
    }
 
    return baseRent * festivalMultiplier;
}
 
bool StreetProperty::canBuild() const {
    return buildingCount < 5;
}
 
void StreetProperty::build() {
    if (!canBuild()) {
        throw runtime_error("Properti " + code + " sudah mencapai level bangunan maksimum.");
    }
    ++buildingCount;
}
 
 
// Sell ALL buildings on this property at once; returns total refund.
int StreetProperty::sellAllBuildings() {
    int total = 0;
    while (buildingCount > 0) {
        total += sellBuilding();
    }
    return total;
}
 
// Returns cost to build the NEXT level
int StreetProperty::getBuildCost() const {
    if (buildingCount < 4) return housePrice;
    if (buildingCount == 4) return hotelPrice;
    return 0; // already hotel
}
 
int StreetProperty::getTotalValue() const {
    // Cash value: buy price + build prices paid (not halved)
    int buildValue = 0;
    int houses = min(buildingCount, 4);
    buildValue += houses * housePrice;
    if (buildingCount == 5) buildValue += hotelPrice;
    return price + buildValue;
}
 
int StreetProperty::getSellValue() const {
    // Sell to bank: buy price + half of building value
    int buildRefund = 0;
    int houses = min(buildingCount, 4);
    buildRefund += (houses * housePrice) / 2;
    if (buildingCount == 5) buildRefund += hotelPrice / 2;
    return price + buildRefund;
}
 
bool StreetProperty::isMonopolized() const {
    // This flag is set externally by the Game/Board after checking all
    // properties in the color group. We expose a method for the tile layer
    // to query; the actual check lives in Player::getPropertiesByColor().
    // Here we just report whether buildingCount logic applies.
    // Actual monopoly detection delegated to caller — see calculateRent().
    return false; // placeholder; real check is done in Game/Tile layer
}