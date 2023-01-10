#include "app.hpp"
#include "pipeline.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vkEngine {

App::App() {
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

App::~App() {
    vkDestroyPipelineLayout(vkEngineDevice.device(), pipelineLayout, nullptr);
}

void App::run() {
  while (!window.shouldClose()) {
    glfwPollEvents();
  }
}

void App::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(vkEngineDevice.device(), &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout");
  }
}

void App::createPipeline() {
  auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(
      vkEngineSwapChain.width(), vkEngineSwapChain.height());
  pipelineConfig.renderPass = vkEngineSwapChain.getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  pipeline = std::make_unique<Pipeline>(vkEngineDevice, "shaders/vert.spv",
                                        "shaders/frag.spv", pipelineConfig);
}

void App::createCommandBuffers() {};
void App::drawFrame() {};
} // namespace vkEngine