#pragma once

#include "Property.hpp"

class RailroadProperty : public Property {
public:
    int calculateRent(int diceTotal, int ownedCountOfType) override;
};
