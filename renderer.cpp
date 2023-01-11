#include "renderer.hpp"
#include "device.hpp"
#include "window.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

namespace vkEngine {

VkEngineRenderer::VkEngineRenderer(Window &window, VkEngineDevice &device)
    : window{window}, vkEngineDevice{device} {
  recreateSwapchain();
  createCommandBuffers();
}

VkEngineRenderer::~VkEngineRenderer() { freeCommandBuffers(); }

void VkEngineRenderer::recreateSwapchain() {
  auto extent = window.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = window.getExtent();
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(vkEngineDevice.device());

  if (vkEngineSwapChain == nullptr) {
    vkEngineSwapChain =
        std::make_unique<VkEngineSwapChain>(vkEngineDevice, extent);
  } else {
    vkEngineSwapChain = std::make_unique<VkEngineSwapChain>(
        vkEngineDevice, extent, std::move(vkEngineSwapChain));
    if (vkEngineSwapChain->imageCount() != commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }
  // brb
}

void VkEngineRenderer::createCommandBuffers() {
  commandBuffers.resize(vkEngineSwapChain->imageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = vkEngineDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(vkEngineDevice.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void VkEngineRenderer::freeCommandBuffers() {
  vkFreeCommandBuffers(vkEngineDevice.device(), vkEngineDevice.getCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

VkCommandBuffer VkEngineRenderer::beginFrame() {
  assert(!isFrameStarted && "Can't call beginFrame while already in progress");
  auto result = vkEngineSwapChain->acquireNextImage(&currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapchain();
    return nullptr;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image");
  }

  isFrameStarted = true;

  auto commandBuffer = getCurrentCommandBuffer();
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  return commandBuffer;
}

void VkEngineRenderer::endFrame() {
  assert(isFrameStarted && "Can't call endFrame while frame is not started");
  auto commandBuffer = getCurrentCommandBuffer();
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
  auto result = vkEngineSwapChain->submitCommandBuffers(&commandBuffer,
                                                        &currentImageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      window.wasWindowResized()) {
    window.resetWindowResizedFlag();
    recreateSwapchain();
  }
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swapchain image");
  }

  isFrameStarted = false;
}
void VkEngineRenderer::beginSwapChainrenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted && "Can't call beginFrame while frame is not started");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't begin renderpass on commandBuffer from different frame");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vkEngineSwapChain->getRenderPass();
  renderPassInfo.framebuffer =
      vkEngineSwapChain->getFrameBuffer(currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = vkEngineSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width =
      static_cast<float>(vkEngineSwapChain->getSwapChainExtent().width);
  viewport.height =
      static_cast<float>(vkEngineSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, vkEngineSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void VkEngineRenderer::endSwapChainrenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted && "Can't call endFrame while frame is not started");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't end renderpass on commandBuffer from different frame");
  vkCmdEndRenderPass(commandBuffer);
}
} // namespace vkEngine