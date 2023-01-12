
#pragma once

#include "camera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace vkEngine {
struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  VkEngineCamera &camera;
};
}  // namespace lve