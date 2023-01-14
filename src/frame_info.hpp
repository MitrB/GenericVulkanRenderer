
#pragma once

#include "camera.hpp"
#include "game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace vkEngine {
struct FrameInfo {
  int frameIndex;
  float frameTime;
  VkCommandBuffer commandBuffer;
  VkEngineCamera &camera;
  VkDescriptorSet globalDescriptorSet;
  VkEngineGameObject::Map &gameObject;
};
}  // namespace vkEngine