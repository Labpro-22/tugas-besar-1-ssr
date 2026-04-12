#pragma once

#include <sstream>
#include <string>

enum class PropertyStatus {
    BANK,
    OWNED,
    MORTGAGED
};

class Property {
protected:
    int ID;
    std::string code;
    std::string name;
    std::string type;
    int price;
    int mortgageValue;
    int ownerID;
    PropertyStatus status;

    virtual int calculateRent(int diceTotal, int ownedCountOfType) = 0;

public:
    virtual ~Property() = default;

    int mortgage();
    int unmortgage();

    virtual int getTotalValue() const;
    virtual int getSellValue() const;

    virtual void printCertificate(std::stringstream& output) const;
};
