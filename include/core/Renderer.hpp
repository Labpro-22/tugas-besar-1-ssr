#pragma once

#include <vector>

#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "EntityRenderer.hpp"

class Renderer {
private:
    raylib::Window* window;
    float angleX = 0.0f;
    float angleY = 0.0f;
    Camera3D camera;
    std::vector<EntityRenderer*> entityRenderers;

public:
    Renderer(const char* title) : Renderer(title, 800, 600) {};
    Renderer(const char* title, int screenWidth, int screenHeight) 
        : angleX(0.0f), angleY(0.0f), camera({.position = Vector3{5.0f, 5.0f, 5.0f},
                     .target = Vector3{0.0f, 0.0f, 0.0f},
                     .up = Vector3{0.0f, 1.0f, 0.0f},
                     .fovy = 45.0f,
                     .projection = CAMERA_PERSPECTIVE}) {
        window = new raylib::Window(screenWidth, screenHeight, title);
    }
    void init();
    void render();

    Renderer& WithEntityRenderer(EntityRenderer* entityRenderer) {
        entityRenderers.push_back(entityRenderer);
        return *this;
    }

    ~Renderer() {
        delete window;
    }
};