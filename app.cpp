#include "app.hpp"
#include <GLFW/glfw3.h>

namespace vkEngine {

void App::run() {
    while (!window.shouldClose()) {
        glfwPollEvents();
    }
}
}