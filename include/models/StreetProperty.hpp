#pragma once

#include <vector>

#include <raylib-cpp.hpp>

#include "Property.hpp"

class StreetProperty : public Property {
public:
    raylib::Color colorGroup;
    int housePrice;
    int hotelPrice;
    std::vector<int> rentLevels;
    int buildingCount;
    int festivalMultiplier;
    int festivalDuration;

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
