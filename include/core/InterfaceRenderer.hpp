#pragma once

#include "raylib-cpp.hpp"

class InterfaceRenderer {
 public:
  class Element {
   public:
    Element(InterfaceRenderer* renderer) : renderer(renderer) {}
    virtual int getHeight() const { return 0; };
    virtual void render(int posX, int posY, int width) {};
    virtual ~Element() = default;

   protected:
    InterfaceRenderer* renderer;
  };

  class Text : public Element {
   public:
    Text(InterfaceRenderer* renderer, const char* text, int fontSize, Color color)
        : Element(renderer), text(text), fontSize(fontSize), color(color) {}
    const char* text;
    int fontSize;
    Color color;

    int getHeight() const override { return fontSize; }
    void render(int posX, int posY, int width) override {
      DrawText(text, posX + 10, posY, fontSize, color);
    };
  };

  class Table : public Element {
   public:
    Table(InterfaceRenderer* renderer, const std::vector<std::vector<std::string>>& data, int rowPadding,
          int colPadding)
        : Element(renderer), data(data), rowPadding(rowPadding), colPadding(colPadding) {}
    std::vector<std::vector<std::string>> data;
    int rowPadding;
    int colPadding;

    int getHeight() const override {
      int h = 0;
      for (const auto& row : data) {
        int maxFontSize = 0;
        for (const auto& cell : row) {
          int fontSize = 12;
          if (fontSize > maxFontSize) maxFontSize = fontSize;
        }
        h += maxFontSize + rowPadding;
      }
      return h;
    }
    void render(int posX, int posY, int width) override {
      std::vector<int> colWidths(data[0].size(), 0);
      for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
          int cellWidth = MeasureText(row[i].c_str(), 12) + colPadding * 2;
          if (cellWidth > colWidths[i]) colWidths[i] = cellWidth;
        }
      }

      int currentX = posX;
      int currentY = posY;
      for (const auto& row : data) {
        int maxFontSize = 0;
        for (const auto& cell : row) {
          int fontSize = 12;
          if (fontSize > maxFontSize) maxFontSize = fontSize;
          DrawText(cell.c_str(), currentX + colPadding, currentY, fontSize,
                   BLACK);
          currentX += colWidths[&cell - &row[0]];
        }
        currentY += maxFontSize + rowPadding;
        currentX = posX;
      }
    }
  };

  class Border : public Element {
   public:
    Border(InterfaceRenderer* renderer) : Element(renderer) {}

    int getHeight() const override { return 4; }
    void render(int posX, int posY, int width) override {
      DrawRectangle(posX, posY, width, 2, BLACK);
    }
  };

  class TopBar : public Element {
   public:
    TopBar(InterfaceRenderer* renderer) : Element(renderer) {}

    int getHeight() const override { return 16; }
    void render(int posX, int posY, int width) override {
      DrawRectangle(posX, posY, width, 16, DARKGRAY);
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
          CheckCollisionPointRec(
              GetMousePosition(),
              Rectangle{(float)posX, (float)posY, (float)width, 16})) {
        isDragging = true;
        dragOffset = Vector2{GetMousePosition().x - posX, GetMousePosition().y - posY};
      } else if (isDragging) {
        auto mousePos = GetMousePosition();
        renderer->SetPosition(mousePos.x - dragOffset.x, mousePos.y - dragOffset.y);

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) isDragging = false;
      }
    }

    private:
        bool isDragging = false;
        Vector2 dragOffset;
  };

  int posX = 0;
  int posY = 0;
  int width = 200;
  int height = 0;

  InterfaceRenderer() = default;
  virtual void init() = 0;
  void render();
  bool isMouseOver();
  virtual ~InterfaceRenderer() = default;

 protected:
  std::vector<std::unique_ptr<Element>> elements;

  InterfaceRenderer& SetPosition(int x, int y);
  InterfaceRenderer& SetWidth(int width);
  InterfaceRenderer& AddElement(std::unique_ptr<Element> element);
};