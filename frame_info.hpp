
#pragma once

#include "camera.hpp"
#include "descriptors.hpp"

// lib
#include <vulkan/vulkan.h>

namespace vkEngine {
struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  VkEngineCamera &camera;
  VkDescriptorSet globalDescriptorSet;
};
}  // namespace vkEngine