#pragma once

#include "Property.hpp"

class UtilityProperty : public Property {
public:
    int calculateRent(int diceTotal, int ownedCountOfType) override;
};
