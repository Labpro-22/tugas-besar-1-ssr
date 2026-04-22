#include "Property.hpp"
#include <stdexcept>
#include <iomanip>

RailroadProperty::RailroadProperty(int ID, string code, string name, int price, int mortgageValue)
    : Property(ID, code, name, "RAILROAD", price, mortgageValue) {}
RailroadProperty::~RailroadProperty() {}
 
// ownedCountOfType = number of railroads owned by this property's owner.
// Rent table is loaded externally; here we delegate to a simple lookup
// stored in the Game/Board layer. We accept the pre-calculated rent from
// the caller via ownedCountOfType (already resolved to a rent value).
// Per the design: calculateRent receives diceTotal and ownedCountOfType.
// For Railroad, ownedCountOfType IS the rent amount (resolved by Game).
int RailroadProperty::calculateRent(int diceNum, int ownedCountOfType) {
    // ownedCountOfType carries the pre-resolved rent from the railroad config.
    return ownedCountOfType;
}