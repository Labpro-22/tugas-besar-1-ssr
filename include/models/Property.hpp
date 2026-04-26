#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <raylib-cpp.hpp>
#include "Dice.hpp"
#include "Player.hpp"

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
    raylib::Color colorGroup;
    PropertyStatus status;
    int festivalMultiplier;
    int festivalDuration;
    
    virtual int calculateRent(int diceNum, int ownedCountOfType, bool isMono) = 0;
    
    public:
    Property(int ID, string code, string name, string type, int price, int mortgageValue, raylib::Color color, int festivalMultiplier, int festivalDuration);
    Property(int ID, string code, string name, string type, int price, int mortgageValue, raylib::Color color);
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
    int getFestivalMultiplier() const { return festivalMultiplier;  }
    int getFestivalDuration() const { return festivalDuration; }
    int getRent(int diceNum, int ownedCountOfType) {
        return calculateRent(diceNum, ownedCountOfType, false);
    }
    raylib::Color getColorGroup() { return colorGroup; }


    /*=== Setters ===*/
    void setOwnerID(int playerID){ ownerID = playerID; }
    void setStatus(PropertyStatus status){ this->status = status; }

    /*===FESTIVAL===*/
    void applyFestival();
    void decrementFestival();
    void resetFestival();
    void setFestivalMultiplier(int m) { festivalMultiplier = m; }
    void setFestivalDuration(int d) { festivalDuration = d; }

};



class StreetProperty : public Property {
public:
    int housePrice;
    int hotelPrice;
    vector<int> rentLevels;
    int buildingCount; // 0-4 : rumah, 5 : hotel
    
    StreetProperty(int ID, string code, string name, int price, int mortgageValue, raylib::Color color, int housePrice, int hotelPrice, vector<int> rentLevels);
    ~StreetProperty();

    int mortgage() override;

    int calculateRent(int diceNum, int ownedCountOfType, bool isMono) override;
    int sellBuilding();
    int getBuildCost() const;
    bool canBuild() const;
    void build();
    int sellAllBuildings();

    int getBuildingCount()const {return buildingCount;}
    int getTotalValue() const override;
    int getSellValue() const override;
    bool isMonopolized() const;
};



class RailroadProperty : public Property {
public:
    static std::vector<int> rentTable;
    RailroadProperty(int ID, string code, string name, int price, int mortgageValue, raylib::Color color);
    RailroadProperty(int ID, string code, string name, int price, int mortgageValue);
    ~RailroadProperty();

    int calculateRent(int diceNum, int ownedCountOfType, bool isMono) override;
};



class UtilityProperty : public Property {
public:
    static std::vector<int> multiplierTable;
    UtilityProperty(int ID, string code, string name, int price, int mortgageValue, raylib::Color color);
    UtilityProperty(int ID, string code, string name, int price, int mortgageValue);
    ~UtilityProperty();
    
    int calculateRent(int diceNum, int ownedCountOfType, bool isMono) override;
};
