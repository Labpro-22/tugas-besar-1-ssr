#pragma once

#include <vector>

#include "Component.hpp"

class Container : public Component {
protected:
    std::vector<Component*> children;
    raylib::Color baseColor;

    virtual void refreshLayout() = 0;
    virtual void updateState(raylib::Vector2 currentMousePos, bool isClicking) = 0;
    virtual void draw() = 0;

public:
    Container(raylib::Rectangle bounds, raylib::Color baseColor, bool visible = true);
    virtual ~Container();
    
    raylib::Color getColor();
    void setColor(raylib::Color color);
};