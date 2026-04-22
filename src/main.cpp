#include "raylib-cpp.hpp"
#include "rlgl.h"
#include "Renderer.hpp"
#include "BoardRenderer.hpp"

#include <cmath>

int main(void)
{
    Renderer renderer("Nimonspoli");
    renderer.init();
    renderer.WithEntityRenderer(new BoardRenderer(5.0, 5.0, 15, 11));

    renderer.render();

    return 0;
}