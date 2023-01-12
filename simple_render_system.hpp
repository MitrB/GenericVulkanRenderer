#pragma once

#include "camera.hpp"
#include "device.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"

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

  void renderGameObjects(FrameInfo &frameInfo, std::vector<VkEngineGameObject> &gameObjects);
                         

private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);

  VkEngineDevice &vkEngineDevice;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};

} // namespace vkEngine