#pragma once

#include <vector>
#include "Player.hpp"

class HumanPlayer : public Player {
public:
    int chooseInput(std::vector<int> choices) override;
};