#pragma once

#include <string>

#include "Component.hpp"

class Label : public Component {
private:
    std::string text;
    raylib::Font font;
    raylib::Color color;
    int fontSize;

protected:
    void updateState(raylib::Vector2 currentMousePos, bool isClicking) override;
    void draw() override;

public:
    Label(raylib::Rectangle bounds, std::string text, int fontSize, raylib::Color color, bool visible = true);
    ~Label() = default;
    
    std::string getText();
    void setText(std::string text);
    void setStyle(raylib::Font font, int fontSize, raylib::Color color);
};