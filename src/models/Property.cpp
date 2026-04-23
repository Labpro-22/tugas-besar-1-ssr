#include "Property.hpp"
#include <stdexcept>
#include <iomanip>

Property::Property(int ID, string code, string name, string type, int price, int mortgageValue, raylib::Color colorGroup;): 
        ID(ID), code(code), name(name), type(type), price(price), mortgageValue(mortgageValue), 
        ownerID(-1), status(PropertyStatus::BANK), festivalMultiplier(1), festivalDuration(0), colorGroup(color){}

int Property::mortgage(){
    if(status != PropertyStatus::OWNED){
        throw runtime_error("Properti " + code + " tidak dapat digadaikan (status bukan OWNED).");
    }
    status = PropertyStatus::MORTGAGED;
}

void Property::unmortgage(){
    if(status != PropertyStatus::MORTGAGED){
        throw runtime_error("Properti " + code + " tidak sedang digadaikan.");
    }
    status = PropertyStatus::OWNED;
}

int Property::getTotalValue() const{
    return price;
}
int Property::getSellValue() const{
    return price; //base price
}

void Property::applyFestival() {
    if (festivalMultiplier < 8) {
        festivalMultiplier *= 2;
    }else{
        cout << "Efek sudah maksimum (harga sewa sudah digandakan tiga kali)\n";
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
