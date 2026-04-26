#pragma once

#include "raylib-cpp.hpp"
#include "EntityRenderer.hpp"

class BoardRenderer : public EntityRenderer {
public:
    BoardRenderer(float boardWidth, float boardHeight, int horizontalTileCount, int verticalTileCount)
        : boardWidth(boardWidth), boardHeight(boardHeight), horizontalTileCount(horizontalTileCount), verticalTileCount(verticalTileCount) {}
    void render() override;

private:
    float boardWidth;
    float boardHeight;
    int horizontalTileCount;
    int verticalTileCount;
    void DrawTile(int index, raylib::Color color);
    float getHTileSize() const {
        return boardWidth / horizontalTileCount;
    }
    float getVTileSize() const {
        return boardHeight / verticalTileCount;
    }
};