#include "GameApp.hpp"
#include "Property.hpp"
#include "AppException.hpp"

#include <stdexcept>
#include <iomanip>

StreetProperty::StreetProperty(int ID, string code, string name,
    int price, int mortgageValue, raylib::Color color, int housePrice, int hotelPrice, vector<int> rentLevels)
    : Property(ID, code, name, "STREET", price, mortgageValue, color),
    housePrice(housePrice),
    hotelPrice(hotelPrice),
    rentLevels(rentLevels),
    buildingCount(0) {}
 
StreetProperty::~StreetProperty() {}

int StreetProperty::calculateRent(int diceNum, int ownedCountOfType, bool isMono) {
    int baseRent = rentLevels[0];
    if(buildingCount == 0){
        baseRent = rentLevels[0];
        if(isMono){
            baseRent *= 2;
        }
    } 
    else if(buildingCount <= 5){
        baseRent = rentLevels[buildingCount];
    }
 
    return baseRent * festivalMultiplier;
}
 
bool StreetProperty::canBuild() const {
    return buildingCount < 5;
}
 
void StreetProperty::build() {
    if (!canBuild()) {
        throw GameException("StreetProperty::build", "Properti " + code + " sudah mencapai level bangunan maksimum.");
    }
    ++buildingCount;
}
 

int StreetProperty::sellBuilding() {
    if (buildingCount == 0) {
        throw GameException("StreetProperty::sellBuilding", "Tidak ada bangunan yang dapat dijual di " + code + ".");
    }
    int refund;
    if (buildingCount == 5) {
        refund = hotelPrice / 2;
    } else {
        refund = housePrice / 2;
    }
    --buildingCount;
    return refund;
}

int StreetProperty::sellAllBuildings() {
    int totalRefund = 0;
    while (buildingCount > 0) {
        totalRefund += sellBuilding();
    }
    return totalRefund;
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
    GameSession *game = GameApp::currentSession;
    Player *owner = nullptr;

    for(Player *p : game->getPlayers()){
        if(p->getPlayerIndex() == ownerID){
            owner = p;
            break;
        }
    }
    if(owner == nullptr) throw GameException("isMonopolized", "Property owne could not be resolved");

    const std::vector<Property*> ownedSameColor = owner->getPropertiesByColor(colorGroup);
    const std::vector<int> boardSameColor = game->getBoard()->getTilesByColor(colorGroup);
    return !boardSameColor.empty() && ownedSameColor.size() == boardSameColor.size();
}
