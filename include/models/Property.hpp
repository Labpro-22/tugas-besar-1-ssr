#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <raylib-cpp.hpp>

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
    Property(int ID, std::string code, std::string name, std::string type, int price, int mortgageValue);
    virtual ~Property() = default;

    int mortgage();
    int unmortgage();

    virtual int getTotalValue() const;
    virtual int getSellValue() const;

    virtual void printCertificate(std::stringstream& output) const;
};



class StreetProperty : public Property {
public:
    raylib::Color colorGroup;
    int housePrice;
    int hotelPrice;
    std::vector<int> rentLevels;
    int buildingCount;
    int festivalMultiplier;
    int festivalDuration;

    StreetProperty(int ID, std::string code, std::string name, int price, int mortgageValue, raylib::Color color, int housePrice, int hotelPrice, std::vector<int> rentLevels);
    ~StreetProperty();

    int calculateRent(int diceTotal, int ownedCountOfType) override;

    bool canBuild() const;
    void build();
    int sellBuilding();

    void applyFestival();
    void decrementFestival();
    void resetFestival();

    int getTotalValue() const override;
    int getSellValue() const override;

    bool isMonopolized() const;
};



class RailroadProperty : public Property {
public:
    RailroadProperty(int ID, std::string code, std::string name, int price, int mortgageValue);
    ~RailroadProperty();

    int calculateRent(int diceTotal, int ownedCountOfType) override;
};



class UtilityProperty : public Property {
public:
    UtilityProperty(int ID, std::string code, std::string name, int price, int mortgageValue);
    ~UtilityProperty();
    
    int calculateRent(int diceTotal, int ownedCountOfType) override;
};
