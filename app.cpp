#include "app.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/gtc/constants.hpp>
#include <memory>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vkEngine {

struct SimplePushConstantData {
  glm::mat2 transform{1.f};
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

App::App() {
  loadGameObjects();
  createPipelineLayout();
  recreateSwapchain();
  createCommandBuffers();
}

App::~App() {
  vkDestroyPipelineLayout(vkEngineDevice.device(), pipelineLayout, nullptr);
}

void App::run() {
  while (!window.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(vkEngineDevice.device());
}

void App::sierpinski(std::vector<Model::Vertex> &vertices, int depth,
                     glm::vec2 left, glm::vec2 right, glm::vec2 top) {
  if (depth <= 0) {
    vertices.push_back({top});
    vertices.push_back({right});
    vertices.push_back({left});
  } else {
    auto leftTop = 0.5f * (left + top);
    auto rightTop = 0.5f * (right + top);
    auto leftRight = 0.5f * (left + right);
    sierpinski(vertices, depth - 1, left, leftRight, leftTop);
    sierpinski(vertices, depth - 1, leftRight, right, rightTop);
    sierpinski(vertices, depth - 1, leftTop, rightTop, top);
  }
}

void App::loadGameObjects() {
  std::vector<Model::Vertex> vertices{
      {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  auto lveModel = std::make_shared<Model>(vkEngineDevice, vertices);
    
  // https://www.color-hex.com/color-palette/5361
  std::vector<glm::vec3> colors{
      {1.f, .7f, .73f},
      {1.f, .87f, .73f},
      {1.f, 1.f, .73f},
      {.73f, 1.f, .8f},
      {.73, .88f, 1.f}  //
  };
  for (auto& color : colors) {
    color = glm::pow(color, glm::vec3{2.2f});
  }
  for (int i = 0; i < 40; i++) {
    auto triangle = vkEngineGameObject::createGameObject();
    triangle.model = lveModel;
    triangle.transform2d.scale = glm::vec2(.5f) + i * 0.025f;
    triangle.transform2d.rotation = i * glm::pi<float>() * .0025f;
    triangle.color = colors[i % colors.size()];
    gameObjects.push_back(std::move(triangle));
  }
}

void App::createPipelineLayout() {

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(vkEngineDevice.device(), &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout");
  }
}

void App::createPipeline() {
  assert(vkEngineSwapChain != nullptr &&
         "Cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = vkEngineSwapChain->getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  pipeline = std::make_unique<Pipeline>(vkEngineDevice, "shaders/vert.spv",
                                        "shaders/frag.spv", pipelineConfig);
}

void App::recreateSwapchain() {
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
  createPipeline();
}

void App::createCommandBuffers() {
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

void App::freeCommandBuffers() {
  vkFreeCommandBuffers(vkEngineDevice.device(), vkEngineDevice.getCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

void App::recordCommandBuffer(int imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vkEngineSwapChain->getRenderPass();
  renderPassInfo.framebuffer = vkEngineSwapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = vkEngineSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo,
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
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

  renderGameObjects(commandBuffers[imageIndex]);

  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void App::renderGameObjects(VkCommandBuffer commandBuffer) {

 int i = 0;
  for (auto& obj : gameObjects) {
    i += 1;
    obj.transform2d.rotation =
        glm::mod<float>(obj.transform2d.rotation + 0.001f * i, 2.f * glm::pi<float>());
  }

  pipeline->bind(commandBuffer);

  for (auto &obj : gameObjects) {
    obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

    SimplePushConstantData push{};
    push.offset = obj.transform2d.translation;
    push.color = obj.color;
    push.transform = obj.transform2d.mat2();

    vkCmdPushConstants(commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
    obj.model->bind(commandBuffer);
    obj.model->draw(commandBuffer);
  }
}

void App::drawFrame() {
  uint32_t imageIndex;
  auto result = vkEngineSwapChain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapchain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image");
  }

  recordCommandBuffer(imageIndex);
  result = vkEngineSwapChain->submitCommandBuffers(&commandBuffers[imageIndex],
                                                   &imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      window.wasWindowResized()) {
    window.resetWindowResizedFlag();
    recreateSwapchain();
    return;
  }
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swapchain image");
  }
}
} // namespace vkEngine