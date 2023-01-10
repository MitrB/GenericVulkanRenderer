#pragma once

#include <string>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vkEngine {

    class Window {
        
        public:
            Window(int widht, int height, std::string windowName);
            ~Window();

            Window(const Window &) = delete;
            Window &operator=(const Window &) = delete;

            bool shouldClose() {return glfwWindowShouldClose(window);}
            void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);


        private:
            void initWindow();

            const int width;
            const int height;
            
            std::string windowName;
            GLFWwindow *window;
    };

}