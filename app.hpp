#pragma once

#include "window.hpp"
#include <GLFW/glfw3.h>

namespace vkEngine {
class App {
    public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();

    private:
    Window window{WIDTH, HEIGHT, "Vulkan Engine"};
};

} // namespace vkEngine