#include <cmath>

#include "raylib-cpp.hpp"
#include "rlgl.h"


#include "Renderer.hpp"
#include "AktaInterface.hpp"
#include "BoardRenderer.hpp"

int main(void) {
  Renderer renderer("Nimonspoli");
  renderer.WithEntityRenderer(new BoardRenderer(5.0, 5.0, 15, 11))
  .WithInterfaceRenderer(new AktaInterface(10, 10));
  renderer.init();

  renderer.render();

  return 0;
}