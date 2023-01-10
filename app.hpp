#pragma once

#include "window.hpp"
#include "pipeline.hpp"

namespace vkEngine {
class App {
    public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();

    private:
    Window window{WIDTH, HEIGHT, "Vulkan Engine"};
    Pipeline pipeline{"shaders/vert.spv", "shaders/frag.spv"};
};

} // namespace vkEngine