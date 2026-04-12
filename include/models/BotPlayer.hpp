#pragma once

#include <vector>
#include "Player.hpp"

class BotPlayer : public Player {
public:
    int chooseInput(std::vector<int> choices) override;
};