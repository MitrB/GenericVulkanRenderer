#pragma once

#include "device.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vkEngine {
class VkEngineRenderer {
public:
  VkEngineRenderer(Window &window, VkEngineDevice &device);
  ~VkEngineRenderer();

  VkEngineRenderer(const VkEngineRenderer &) = delete;
  VkEngineRenderer &operator=(const VkEngineRenderer &) = delete;

  VkRenderPass getSwapChainrenderPass() const {
    return vkEngineSwapChain->getRenderPass();
  }
  bool isFrameInProgress() const { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted &&
           "Cannot get command buffer when frame not in progress");
    return commandBuffers[currentImageIndex];
  }

  VkCommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainrenderPass(VkCommandBuffer commandBuffer);
  void endSwapChainrenderPass(VkCommandBuffer commandBuffer);

private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapchain();

  Window &window;
  VkEngineDevice &vkEngineDevice;
  std::unique_ptr<VkEngineSwapChain> vkEngineSwapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex;
  bool isFrameStarted = false;
};

} // namespace vkEngine