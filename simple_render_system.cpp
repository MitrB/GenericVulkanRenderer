#include "simple_render_system.hpp"
#include "camera.hpp"
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
  glm::mat4 transform{1.f};
  alignas(16) glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(VkEngineDevice &device, VkRenderPass renderPass) : vkEngineDevice{device}{
  createPipelineLayout();
  createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
  vkDestroyPipelineLayout(vkEngineDevice.device(), pipelineLayout, nullptr);
}


void SimpleRenderSystem::createPipelineLayout() {

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

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  pipeline = std::make_unique<Pipeline>(vkEngineDevice, "shaders/vert.spv",
                                        "shaders/frag.spv", pipelineConfig);
}


void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VkEngineGameObject> &gameObjects, const VkEngineCamera &camera) {

  pipeline->bind(commandBuffer);

  auto projecionView= camera.getProjection() * camera.getView();

  for (auto &obj : gameObjects) {
    // TODO:
    obj.transform.rotation.z = glm::mod(obj.transform.rotation.z + 0.009f, glm::two_pi<float>());
    obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.001f, glm::two_pi<float>());
    obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.0005f, glm::two_pi<float>());

    SimplePushConstantData push{};
    push.color = obj.color;
    push.transform = projecionView * obj.transform.mat4();

    vkCmdPushConstants(commandBuffer, pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(SimplePushConstantData), &push);
    obj.model->bind(commandBuffer);
    obj.model->draw(commandBuffer);
  }
}

}