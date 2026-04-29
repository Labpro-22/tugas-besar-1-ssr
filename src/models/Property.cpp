#include "Property.hpp"
#include "AppException.hpp"
#include "GameApp.hpp"
#include "GameSession.hpp"

#include <stdexcept>
#include <iomanip>

Property::Property(int ID, string code, string name, string type, int price, int mortgageValue, raylib::Color colorGroup): 
        ID(ID), code(code), name(name), type(type), price(price), mortgageValue(mortgageValue), 
        ownerID(-1), status(PropertyStatus::BANK), festivalMultiplier(1), festivalDuration(0), colorGroup(colorGroup){}

int Property::mortgage(){
    if(status != PropertyStatus::OWNED){
        throw GameException("Property::mortgage", "Properti " + code + " tidak dapat digadaikan (status bukan OWNED).");
    }
    status = PropertyStatus::MORTGAGED;
    return mortgageValue;
}

int Property::unmortgage(){
    if(status != PropertyStatus::MORTGAGED){
        throw GameException("Property::unmortgage", "Properti " + code + " tidak sedang digadaikan.");
    }
    status = PropertyStatus::OWNED;
    return 0;
}

int Property::getTotalValue() const{
    return price;
}
int Property::getSellValue() const{
    return price;
}

void Property::applyFestival(int multiplier, int duration) {

    GameSession *game = GameApp::currentSession;

    int maxMultiplier = 1;
    for(int i = 0; i < duration; i++) maxMultiplier *= multiplier;

    if (festivalMultiplier < maxMultiplier) {
        festivalMultiplier *= multiplier;
        cout << "Efek festival memberikan bonus " << festivalMultiplier << " kali lipat pada properti tersebut.\n"; 
        game->log("FESTIVAL", "Efek festival berhasil diterapkan dengan x" + std::to_string(festivalMultiplier) + " kali lipat penggandaan");
    }
    else{
        cout << "Efek sudah maksimum (harga sewa sudah digandakan sebanyak " << duration << " kali)\n";
        game->log("FESTIVAL", "Efek festival gagal diterapkan karena properti terkait sudah memiliki x" + std::to_string(festivalMultiplier) + " kali lipat penggandaan");
    }

    festivalDuration = duration;
    cout << "Durasi efek festival berubah menjadi bersisa " << festivalDuration << " giliran.\n"; 
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
