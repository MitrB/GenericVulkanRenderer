#include "app.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "simple_render_system.hpp"
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

App::App() { loadGameObjects(); }

App::~App() {}

void App::run() {
  SimpleRenderSystem simpleRenderSystem(
      vkEngineDevice, vkEngineRenderer.getSwapChainrenderPass());

  while (!window.shouldClose()) {
    glfwPollEvents();

    if (auto commandBuffer = vkEngineRenderer.beginFrame()) {
      vkEngineRenderer.beginSwapChainrenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
      vkEngineRenderer.endSwapChainrenderPass(commandBuffer);
      vkEngineRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(vkEngineDevice.device());
}

void App::loadGameObjects() {
  std::vector<Model::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  auto lveModel = std::make_shared<Model>(vkEngineDevice, vertices);

  // https://www.color-hex.com/color-palette/5361
  std::vector<glm::vec3> colors{
      {1.f, .7f, .73f},
      {1.f, .87f, .73f},
      {1.f, 1.f, .73f},
      {.73f, 1.f, .8f},
      {.73, .88f, 1.f} //
  };
  for (auto &color : colors) {
    color = glm::pow(color, glm::vec3{2.2f});
  }
  for (int i = 0; i < 40; i++) {
    auto triangle = VkEngineGameObject::createGameObject();
    triangle.model = lveModel;
    triangle.transform2d.scale = glm::vec2(.5f) + i * 0.025f;
    triangle.transform2d.rotation = i * glm::pi<float>() * .0025f;
    triangle.color = colors[i % colors.size()];
    gameObjects.push_back(std::move(triangle));
  }
}
} // namespace vkEngine