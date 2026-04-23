#include "InterfaceRenderer.hpp"

void InterfaceRenderer::render() {
    DrawRectangle(posX, posY, width, height, LIGHTGRAY);
    DrawRectangleLines(posX, posY, width, height, BLACK);
    int currentY = posY;
    for (const auto& element : elements) {
        element->render(posX, currentY, width);
        currentY += element->getHeight();
    }
}

InterfaceRenderer& InterfaceRenderer::SetPosition(int x, int y) {
    this->posX = x;
    this->posY = y;
    return *this;
}

InterfaceRenderer& InterfaceRenderer::SetWidth(int width) {
    this->width = width;
    return *this;
}

InterfaceRenderer& InterfaceRenderer::AddElement(std::unique_ptr<Element> element) {
    height += element->getHeight();
    elements.push_back(std::move(element));
    return *this;
}

bool InterfaceRenderer::isMouseOver() {
    return CheckCollisionPointRec(GetMousePosition(),
                                  Rectangle{(float)posX, (float)posY, (float)width, (float)height});
}