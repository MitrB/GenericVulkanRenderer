#include "app.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "model.hpp"
#include "simple_render_system.hpp"
#include "camera.hpp"
#include "keyboard_movement_controller.hpp"

// std
#include <array>
#include <cstdint>
#include <glm/trigonometric.hpp>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <iostream>
#include <chrono>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace vkEngine {

App::App() {
  loadGameObjects();
}

App::~App() {}

void App::run() {
  // for (VkEngineGameObject &object : gameObjects) {
  //   std::cout << object.getId() << std::endl;
  // }
  SimpleRenderSystem simpleRenderSystem(
      vkEngineDevice, vkEngineRenderer.getSwapChainrenderPass());

  VkEngineCamera camera{};
  camera.setViewTarget(glm::vec3{-1.f, -2.f, 2.f}, glm::vec3{0.f, 0.f, 2.5f});

  auto viewerObject = VkEngineGameObject::createGameObject();
  KeyBoardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();
  while (!window.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    delta =glm::min(delta, MAX_FRAME_TIME);

    cameraController.moveInPlaneXZ(window.getGLFWwindow(), delta, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
    
    float aspect = vkEngineRenderer.getAspectRatio();
    // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);

    if (auto commandBuffer = vkEngineRenderer.beginFrame()) {
      vkEngineRenderer.beginSwapChainrenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
      vkEngineRenderer.endSwapChainrenderPass(commandBuffer);
      vkEngineRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(vkEngineDevice.device());
}
// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<Model> createCubeModel(VkEngineDevice& device, glm::vec3 offset) {
  std::vector<Model::Vertex> vertices{

      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      
      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

  };
  for (auto& v : vertices) {
    v.position += offset;
  }
  return std::make_unique<Model>(device, vertices);
}

void App::loadGameObjects() {
  std::shared_ptr<Model> cubeModel = createCubeModel(vkEngineDevice, {.0f, .0f, .0f});
  auto cube1 = VkEngineGameObject::createGameObject();
  cube1.model = cubeModel;
  cube1.transform.translation = {.0f, .0f, 2.5f};
  cube1.transform.scale = {.5f, .5f, .5f};

  // auto cube2 = VkEngineGameObject::createGameObject();
  // cube2.model = cubeModel;
  // cube2.transform.translation = {-.5f, .0f, .5f};
  // cube2.transform.scale = {.5f, .3f, .5f};
  // gameObjects.push_back(std::move(cube2));
  gameObjects.push_back(std::move(cube1));
}


} // namespace vkEngine