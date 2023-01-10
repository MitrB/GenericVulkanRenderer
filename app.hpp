#pragma once

#include "device.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

// std
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vkEngine {
class App {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  App();
  ~App();

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  void run();

private:
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();

  Window window{WIDTH, HEIGHT, "Vulkan Engine"};
  VkEngineDevice vkEngineDevice{window};
  VkEngineSwapChain vkEngineSwapChain{vkEngineDevice, window.getExtent()};
  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
};

} // namespace vkEngine