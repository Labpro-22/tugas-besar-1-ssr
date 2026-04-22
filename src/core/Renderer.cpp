#include "Renderer.hpp"

void Renderer::init() {
  SetTargetFPS(60);
}

void Renderer::render() {
  while (!window->ShouldClose()) {
    BeginDrawing();
        window->ClearBackground(RAYWHITE);

        // 3d initialize
        BeginMode3D(camera);
        
        // Draw plane
        rlPushMatrix();
            rlRotatef(angleX * RAD2DEG, 1.0f, 0.0f, 0.0f);
            rlRotatef(angleY * RAD2DEG, 0.0f, 1.0f, 0.0f);

            // Entities
            for (EntityRenderer* entityRenderer : entityRenderers) entityRenderer->render();
            rlEnd();
        rlPopMatrix();

        // Update angles
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouseDelta = GetMouseDelta();
            angleY += mouseDelta.x * 0.01f;
        }

        // Zoom
        camera.fovy -= GetMouseWheelMove() * 2.0f;
        if (camera.fovy < 10.0f) camera.fovy = 10.0f;
        if (camera.fovy > 120.0f) camera.fovy = 120.0f;

        DrawFPS(10, 10);

        EndMode3D();
    EndDrawing();
  }
}