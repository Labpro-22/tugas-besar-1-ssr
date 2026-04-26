#pragma once

#include "raylib-cpp.hpp"
#include "InterfaceRenderer.hpp"

class AktaInterface : public InterfaceRenderer {
public:
    AktaInterface(int posX, int posY) {
        SetPosition(posX, posY);
    };
    void init() override;
    ~AktaInterface() = default;
};