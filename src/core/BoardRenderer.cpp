#include "BoardRenderer.hpp"

#include <vector>
#include <cmath>

// dummy data for each monopoly tile (22)
std::vector<raylib::Color> tileColors = {
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, GRAY,
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, GRAY,
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, GRAY,
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, GRAY,
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, GRAY,
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, GRAY,
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, GRAY,
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, GRAY,
    RED, BLUE, GREEN, YELLOW, ORANGE, PURPLE, BROWN, GRAY,
};

#define TILE_HEIGHT 0.1f

void BoardRenderer::DrawTile(int index, raylib::Color color) {
    float tileHSize = getHTileSize();
    float tileVSize = getVTileSize();

    if (index % (horizontalTileCount + verticalTileCount - 2) == 0) {  // corner tile - horizontal
    rlPushMatrix();
    rlRotatef(180.0f * (index / (horizontalTileCount + verticalTileCount - 2)), 0.0f, 0.25f, 0.0f);
    DrawCube({(boardWidth - tileVSize) / 2.0f, 0.5f + TILE_HEIGHT / 2.0f,
              (boardHeight - tileHSize) / 2.0f},
             tileVSize, TILE_HEIGHT, tileHSize, color);
    rlPopMatrix();
  } else if (index % (horizontalTileCount + verticalTileCount - 2) == horizontalTileCount - 1) {  // corner tile - vertical
    rlPushMatrix();
    rlRotatef(90.0 + 180.0f * (index / (horizontalTileCount + verticalTileCount - 2)), 0.0f, 0.25f, 0.0f);
    DrawCube({(boardWidth - tileHSize) / 2.0f, 0.5f + TILE_HEIGHT / 2.0f,
              (boardHeight - tileVSize) / 2.0f},
             tileHSize, TILE_HEIGHT, tileVSize, color);
    rlPopMatrix();
  } else {
    if (index % (horizontalTileCount + verticalTileCount - 2) < horizontalTileCount - 1) {  // horizontal tile
      rlPushMatrix();
      rlRotatef(180.0f * (index / (horizontalTileCount + verticalTileCount - 2)), 0.0f, 0.25f, 0.0f);
      DrawCube({(boardHeight - tileVSize) / 2.0f - tileVSize / 4.0f, 0.5f + TILE_HEIGHT / 2.0f,
                (boardHeight - tileHSize) / 2.0f - (index % (horizontalTileCount + verticalTileCount - 2)) * tileHSize},
               tileVSize / 2, TILE_HEIGHT, tileHSize, color);
      DrawCube({(boardHeight - tileVSize) / 2.0f + tileVSize / 4.0f, 0.5f + TILE_HEIGHT / 2.0f,
                (boardHeight - tileHSize) / 2.0f - (index % (horizontalTileCount + verticalTileCount - 2)) * tileHSize},
               tileVSize / 2, TILE_HEIGHT, tileHSize, WHITE);
      rlPopMatrix();
    } else {  // vertical tile
        rlPushMatrix();
        rlRotatef(90.0 + 180.0f * (index / (horizontalTileCount + verticalTileCount - 2)), 0.0f, 0.25f, 0.0f);
        DrawCube({(boardHeight - tileHSize) / 2.0f - tileHSize / 4.0f, 0.5f + TILE_HEIGHT / 2.0f,
                  (boardHeight - tileVSize) / 2.0f - ((index - horizontalTileCount + 1) % (horizontalTileCount + verticalTileCount - 2)) * tileVSize},
                 tileHSize / 2, TILE_HEIGHT, tileVSize, color);
        DrawCube({(boardHeight - tileHSize) / 2.0f + tileHSize / 4.0f, 0.5f + TILE_HEIGHT / 2.0f,
                  (boardHeight - tileVSize) / 2.0f - ((index - horizontalTileCount + 1) % (horizontalTileCount + verticalTileCount - 2)) * tileVSize},
                 tileHSize / 2, TILE_HEIGHT, tileVSize, WHITE); 
        rlPopMatrix();
    }
  }
}

void BoardRenderer::render() {
  DrawCube({0.0f, 0.0f, 0.0f}, boardWidth, 1.0f, boardHeight, LIGHTGRAY);
  for (int i = 0; i < fmin(tileColors.size(), horizontalTileCount * 2 + verticalTileCount * 2 - 4); i++) {
    DrawTile(i, tileColors[i % tileColors.size()]);
  }
}