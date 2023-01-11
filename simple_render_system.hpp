#pragma once

#include "device.hpp"
#include "pipeline.hpp"
#include "game_object.hpp"

// std
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vkEngine {
class SimpleRenderSystem {
public:
  SimpleRenderSystem(VkEngineDevice &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

  void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VkEngineGameObject> &gameObjects);

private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);

  VkEngineDevice &vkEngineDevice;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};

}