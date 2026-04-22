#include "Property.hpp"
#include <stdexcept>
#include <iomanip>

Property::Property(int ID, string code, string name, string type, int price, int mortgageValue): 
        ID(ID), code(code), name(name), type(type), price(price), mortgageValue(mortgageValue), ownerID(-1), status(PropertyStatus::BANK) {}
virtual ~Property() = default;

int Property::mortgage(){
    if(status != PropertyStatus::OWNED){
        throw runtime_error("Properti " + code + " tidak dapat digadaikan (status bukan OWNED).");
    }
    status = PropertyStatus::MORTGAGED;
    return mortgageValue;
}
int Property::unmortgage(){
    if(status != PropertyStatus::MORTGAGED){
        throw runtime_error("Properti " + code + " tidak sedang digadaikan.");
    }
    status = PropertyStatus::OWNED;
    return price;
}

int Property::getTotalValue() const{
    return price;
}
int Property::getSellValue() const{
    return price; //base price
}

void Property::applyFestival() {
    // Per spec: max 3 doublings (multiplier 1→2→4→8).
    // Resets duration regardless; only multiplies if below max.
    if (festivalMultiplier < 8) {
        festivalMultiplier *= 2;
    }
    festivalDuration = 3;
}
 
void Property::decrementFestival() {
    if (festivalDuration > 0) {
        --festivalDuration;
        if (festivalDuration == 0) {
            resetFestival();
        }
    }
}
 
void Property::resetFestival() {
    festivalMultiplier = 1;
    festivalDuration   = 0;
}

void Property::printCertificate(stringstream& output) const {
    output << "+================================+\n";
    output << "|       AKTA KEPEMILIKAN         |\n";
    output << "| " << left << setw(30) << (name + " (" + code + ")") << "|\n";
    output << "+================================+\n";
    output << "| Harga Beli  : M" << left << setw(14) << price << "|\n";
    output << "| Nilai Gadai : M" << left << setw(14) << mortgageValue << "|\n";
    output << "+--------------------------------+\n";
 
    // Status line
    string statusStr;
    switch (status) {
        case PropertyStatus::BANK:       statusStr = "BANK";       break;
        case PropertyStatus::OWNED:      statusStr = "OWNED";      break;
        case PropertyStatus::MORTGAGED:  statusStr = "MORTGAGED";  break;
    }
    output << "| Status      : " << left << setw(17) << statusStr << "|\n";
    output << "+================================+\n";
}
