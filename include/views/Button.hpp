#pragma once

#include <functional>

#include "Component.hpp"
#include "Label.hpp"


template <typename T>
class Button : public Component {
private:
    T baseShape;
    Label label;
    std::function<void()> onHover;
    std::function<void()> onClick;
    std::function<void()> onRelease;
    bool isHovered;
    bool isClicked;

    void draw() override;

protected:
    void updateState(raylib::Vector2 currentMousePos, bool isClicking) override;

public:
    Button(T baseShape, Label label);
    ~Button() = default;
    
    T *getBase();
    Label *getLabel();
    void setOnHover(std::function<void()> callback);
    void setOnClick(std::function<void()> callback);
    void setOnRelease(std::function<void()> callback);
};