#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <raylib-cpp.hpp>
#include "Dice.hpp"

using namespace std;
enum class PropertyStatus {
    BANK,
    OWNED,
    MORTGAGED
};

class Property {
protected:
    int ID;
    string code;
    string name;
    string type;
    int price;
    int mortgageValue;
    int ownerID;
    PropertyStatus status;
    int festivalMultiplier;
    int festivalDuration;

    virtual int calculateRent(int diceNum, int ownedCountOfType) = 0;

public:
    Property(int ID, string code, string name, string type, int price, int mortgageValue);
    virtual ~Property() = default;

    virtual int mortgage();
    int unmortgage();

    virtual int getTotalValue() const;
    virtual int getSellValue() const;
    virtual void printCertificate(stringstream& os) const;

    /*=== Getters ===*/
    int getID() const { return ID; }
    string getCode() const { return code; }
    string getName() const { return name; }
    string getType() const { return type; }
    int getPrice() const { return price; }
    int getMortgageValue() const { return mortgageValue; }
    int getOwnerID() const { return ownerID; }
    PropertyStatus getStatus() const { return status; }
    int getRent(int diceNum, int ownedCountOfType) {
        return calculateRent(diceNum, ownedCountOfType);
    }

    /*===FESTIVAL===*/
    void applyFestival();
    void decrementFestival();
    void resetFestival();

};



class StreetProperty : public Property {
public:
    raylib::Color colorGroup;
    int housePrice;
    int hotelPrice;
    vector<int> rentLevels;
    int buildingCount; // 0-4 : rumah, 5 : hotel

    StreetProperty(int ID, string code, string name, int price, int mortgageValue, raylib::Color color, int housePrice, int hotelPrice, vector<int> rentLevels);
    ~StreetProperty();

    int Property::mortgage() override;

    int calculateRent(int diceNum, int ownedCountOfType) override;
    int sellAllBuildings();
    int getBuildCost() const;
    bool canBuild() const;
    void build();
    int sellAllBuilding();

    int getTotalValue() const override;
    int getSellValue() const override;
    bool isMonopolized() const;
};



class RailroadProperty : public Property {
public:
    RailroadProperty(int ID, string code, string name, int price, int mortgageValue);
    ~RailroadProperty();

    int calculateRent(int diceNum, int ownedCountOfType) override;
};



class UtilityProperty : public Property {
public:
    UtilityProperty(int ID, string code, string name, int price, int mortgageValue);
    ~UtilityProperty();
    
    int calculateRent(int diceNum, int ownedCountOfType) override;
};
