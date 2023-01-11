#pragma once

#include "device.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include "game_object.hpp"

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
  void loadGameObjects();
  void createPipelineLayout();
  void createPipeline();
  void renderGameObjects(VkCommandBuffer commandBuffer);

  void sierpinski(
    std::vector<Model::Vertex> &vertices,
    int depth,
    glm::vec2 left,
    glm::vec2 right,
    glm::vec2 top);

  Window window{WIDTH, HEIGHT, "Vulkan Engine"};
  VkEngineDevice vkEngineDevice{window};
  VkEngineRenderer vkEngineRenderer{window, vkEngineDevice};
  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<vkEngineGameObject> gameObjects;
};

} // namespace vkEngine