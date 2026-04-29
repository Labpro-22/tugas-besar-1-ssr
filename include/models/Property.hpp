#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Dice.hpp"
#include "Player.hpp"

#include "Color.hpp"

using namespace std;
enum class PropertyStatus {
    BANK,
    OWNED,
    MORTGAGED
};

enum class PropertyType {
    STREET,
    RAILROAD,
    UTILITY
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
    Color colorGroup;
    PropertyStatus status;
    int festivalMultiplier;
    int festivalDuration;
    
public:
    Property(int ID, string code, string name, string type, int price, int mortgageValue, Color color, int festivalMultiplier, int festivalDuration);
    Property(int ID, string code, string name, string type, int price, int mortgageValue, Color color);
    Property(int ID, string code, string name, string type, int price, int mortgageValue);

    virtual ~Property() = default;
        
    virtual int calculateRent(int diceNum, int ownedCountOfType, bool isMono) = 0;
    virtual int mortgage();
    
    int unmortgage();
    
    virtual int getTotalValue() const;
    virtual int getSellValue() const;
    virtual void printCertificate(stringstream& os) const;

    /*=== Getters ===*/
    virtual PropertyType getPropertyType() const = 0;
    virtual class StreetProperty* asStreetProperty() { return nullptr; }
    virtual class RailroadProperty* asRailroadProperty() { return nullptr; }
    virtual class UtilityProperty* asUtilityProperty() { return nullptr; }

    virtual int getBuildingCount() const { return 0; }
    virtual int getBuildCost() const { return 0; }
    virtual bool canBuild() const { return false; }
    virtual void build() {}
    virtual int sellAllBuildings() { return 0; }

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
    Color getColorGroup() { return colorGroup; }


    /*=== Setters ===*/
    void setOwnerID(int playerID){ ownerID = playerID; }
    void setStatus(PropertyStatus status){ this->status = status; }

    /*===FESTIVAL===*/
    void applyFestival(int multiplier, int duration);
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
    int buildingCount;
    
    StreetProperty(int ID, string code, string name, int price, int mortgageValue, Color color, int housePrice, int hotelPrice, vector<int> rentLevels);
    ~StreetProperty();

    PropertyType getPropertyType() const override { return PropertyType::STREET; }
    StreetProperty *asStreetProperty() override { return this; }
    int calculateRent(int diceNum, int ownedCountOfType, bool isMono) override;
    int sellBuilding();
    int getBuildCost() const override;
    bool canBuild() const;
    void build();
    int sellAllBuildings() override;

    int getBuildingCount() const override {return buildingCount;}
    int getTotalValue() const override;
    int getSellValue() const override;
    bool isMonopolized() const;
};



class RailroadProperty : public Property {
public:
    RailroadProperty(int ID, string code, string name, int price, int mortgageValue, Color color);
    RailroadProperty(int ID, string code, string name, int price, int mortgageValue);
    ~RailroadProperty();

    PropertyType getPropertyType() const override { return PropertyType::RAILROAD; }
    RailroadProperty *asRailroadProperty() override { return this; }
    int calculateRent(int diceNum, int ownedCountOfType, bool isMono) override;
};



class UtilityProperty : public Property {
public:
    UtilityProperty(int ID, string code, string name, int price, int mortgageValue, Color color);
    UtilityProperty(int ID, string code, string name, int price, int mortgageValue);
    ~UtilityProperty();
    
    PropertyType getPropertyType() const override { return PropertyType::UTILITY; }
    UtilityProperty* asUtilityProperty() override { return this; }
    int calculateRent(int diceNum, int ownedCountOfType, bool isMono) override;
};
;
