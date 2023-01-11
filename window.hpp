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
            VkExtent2D getExtent() {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}
            void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

            bool wasWindowResized() {return framebufferResized;}
            void resetWindowResizedFlag() {framebufferResized = false;}
            GLFWwindow *getGLFWwindow() const {return window;}


        private:
            static void framebufferResizedCallback(GLFWwindow *window, int width, int height);
            void initWindow();

            int width;
            int height;
            bool framebufferResized = false;
            
            std::string windowName;
            GLFWwindow *window;
    };

}