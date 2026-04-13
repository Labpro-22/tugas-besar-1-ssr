#pragma once

#include "Container.hpp"


class VBox : public Container {
private:
    float verticalSpacing;
    void refreshLayout() override;
    void draw() override;

public:
    VBox(raylib::Rectangle bounds, raylib::Color baseColor, float verticalSpacing, bool visible = true);
    ~VBox() = default;

    void add(Component* child, int idx = -1);
    void remove(Component* child, int idx = -1);
    Component* getChild(int idx);
    float getSpacing();
    void setSpacing(float value);
    void updateState(raylib::Vector2 currentMousePos, bool isClicking) override;
};



class HBox : public Container {
private:
    float horizontalSpacing;
    void refreshLayout() override;
    void draw() override;

public:
    HBox(raylib::Rectangle bounds, raylib::Color baseColor, float horizontalSpacing, bool visible = true);
    ~HBox() = default;

    void add(Component* child, int idx = -1);
    void remove(Component* child, int idx = -1);
    Component* getChild(int idx);
    float getSpacing();
    void setSpacing(float value);
    void updateState(raylib::Vector2 currentMousePos, bool isClicking) override;
};



class StackPane : public Container {
private:
    void refreshLayout() override;
    void draw() override;

public:
    StackPane(raylib::Rectangle bounds, raylib::Color baseColor, bool visible = true);
    ~StackPane() = default;

    void push(Component* child);
    Component* pop();
    Component* top();
    void updateState(raylib::Vector2 currentMousePos, bool isClicking) override;
};



class BorderPane : public Container {
private:
    float topWidth;
    float bottomWidth;
    float rightWidth;
    float leftWidth;
    
    Component* centerSlot = nullptr;
    Component* topSlot = nullptr;
    Component* bottomSlot = nullptr;
    Component* rightSlot = nullptr;
    Component* leftSlot = nullptr;

    void refreshLayout() override;
    void draw() override;

public:
    BorderPane(raylib::Rectangle bounds, raylib::Color baseColor, float top, float bottom, float right, float left, bool visible = true);
    ~BorderPane() = default;
    
    std::vector<float> getWidth();
    void setWidth(float top, float bottom, float right, float left);

    Component *getTop();
    Component *getBottom();
    Component *getRight();
    Component *getLeft();

    void setTop(Component *child);
    void setBottom(Component *child);
    void setRight(Component *child);
    void setLeft(Component *child);
    void setCenter(Component *child);
    
    void updateState(raylib::Vector2 currentMousePos, bool isClicking) override;
};