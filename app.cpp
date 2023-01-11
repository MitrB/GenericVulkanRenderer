#include "app.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "model.hpp"
#include "simple_render_system.hpp"

// std
#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace vkEngine {

App::App() {
  // loadGameObjects();
}

App::~App() {}

std::unique_ptr<Model> createSquareModel(VkEngineDevice &device,
                                         glm::vec2 offset) {
  std::vector<Model::Vertex> vertices = {
      {{-0.5f, -0.5f}}, {{0.5f, 0.5f}},  {{-0.5f, 0.5f}},
      {{-0.5f, -0.5f}}, {{0.5f, -0.5f}}, {{0.5f, 0.5f}}, //
  };
  for (auto &v : vertices) {
    v.position += offset;
  }
  return std::make_unique<Model>(device, vertices);
}

std::unique_ptr<Model> createCircleModel(VkEngineDevice &device,
                                         unsigned int numSides) {
  std::vector<Model::Vertex> uniqueVertices{};
  for (int i = 0; i < numSides; i++) {
    float angle = i * glm::two_pi<float>() / numSides;
    uniqueVertices.push_back({{glm::cos(angle), glm::sin(angle)}});
  }
  uniqueVertices.push_back({}); // adds center vertex at 0, 0

  std::vector<Model::Vertex> vertices{};
  for (int i = 0; i < numSides; i++) {
    vertices.push_back(uniqueVertices[i]);
    vertices.push_back(uniqueVertices[(i + 1) % numSides]);
    vertices.push_back(uniqueVertices[numSides]);
  }
  return std::make_unique<Model>(device, vertices);
}

void App::run() {
  // Creating object to be displayed
  std::shared_ptr<Model> squareModel =
      createSquareModel(vkEngineDevice, {0.5, 0.f});
  std::vector<VkEngineGameObject> squares{};
  auto square1 = VkEngineGameObject::createGameObject();
  square1.model = squareModel;
  square1.color = {1.f, 1.f, 1.f};
  square1.transform2d.rotation = 0;
  square1.transform2d.scale = {.5f, .5f};

  gameObjects.push_back(std::move(square1));

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