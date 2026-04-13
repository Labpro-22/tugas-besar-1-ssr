#pragma once

#include <raylib-cpp.hpp>

class Component {
protected:
    raylib::Rectangle bounds;
    bool visible;

    virtual void updateState(raylib::Vector2 currentMousePos, bool isClicking) = 0;
    virtual void draw() = 0;

public:
    Component(raylib::Rectangle bounds, bool visible = true);
    virtual ~Component() = default;
    
    raylib::Rectangle getBounds();
    void setPosition(raylib::Vector2 pos);
    void setVisible(bool state);
};