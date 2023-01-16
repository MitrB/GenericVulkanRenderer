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
class PointLightSystem {
public:
  PointLightSystem(VkEngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout);
  ~PointLightSystem();

  PointLightSystem(const PointLightSystem &) = delete;
  PointLightSystem &operator=(const PointLightSystem &) = delete;

  void render(FrameInfo &frameInfo);
                         

private:
  void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
  void createPipeline(VkRenderPass renderPass);

  VkEngineDevice &vkEngineDevice;

  std::unique_ptr<Pipeline> pipeline;
  VkPipelineLayout pipelineLayout;
};

} // namespace vkEngine