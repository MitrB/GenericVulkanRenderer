#include "app.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "device.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"
#include "model.hpp"
#include "simple_render_system.hpp"
#include "swap_chain.hpp"

// std
#include <array>
#include <chrono>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace vkEngine {

struct GlobalUbo {
  glm::mat4 projecionView{1.f};
  glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
};

App::App() { loadGameObjects(); }

App::~App() {}

void App::run() {
  // for (VkEngineGameObject &object : gameObjects) {
  //   std::cout << object.getId() << std::endl;
  // }

  VkEngineBuffer globalUboBuffer{
      vkEngineDevice,
      sizeof(GlobalUbo),
      VkEngineSwapChain::MAX_FRAMES_IN_FLIGHT,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      vkEngineDevice.properties.limits.minUniformBufferOffsetAlignment,
  };
  globalUboBuffer.map();

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
    float delta = std::chrono::duration<float, std::chrono::seconds::period>(
                      newTime - currentTime)
                      .count();
    currentTime = newTime;

    delta = glm::min(delta, MAX_FRAME_TIME);

    cameraController.moveInPlaneXZ(window.getGLFWwindow(), delta, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    float aspect = vkEngineRenderer.getAspectRatio();
    // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);

    if (auto commandBuffer = vkEngineRenderer.beginFrame()) {
      int frameIndex = vkEngineRenderer.getFrameIndex();
      FrameInfo frameInfo{frameIndex, delta, commandBuffer, camera};

      // update
      GlobalUbo ubo{};
      ubo.projecionView = camera.getProjection() * camera.getView();
      globalUboBuffer.writeToIndex(&ubo, frameIndex);
      globalUboBuffer.flushIndex(frameIndex);

      // render
      vkEngineRenderer.beginSwapChainrenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
      vkEngineRenderer.endSwapChainrenderPass(commandBuffer);
      vkEngineRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(vkEngineDevice.device());
}
// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<Model> createCubeModel(VkEngineDevice &device,
                                       glm::vec3 offset) {
  Model::Builder modelBuilder{};
  modelBuilder.vertices = {

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
  for (auto &v : modelBuilder.vertices) {
    v.position += offset;
  }
  return std::make_unique<Model>(device, modelBuilder);
}

// temporary helper function, creates a 1x1x1 cube centered at offset with an
// index buffer
std::unique_ptr<Model> createCubeModelIndexed(VkEngineDevice &device,
                                              glm::vec3 offset) {
  Model::Builder modelBuilder{};
  modelBuilder.vertices = {
      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
  };
  for (auto &v : modelBuilder.vertices) {
    v.position += offset;
  }

  modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,
                          8,  9,  10, 8,  11, 9,  12, 13, 14, 12, 15, 13,
                          16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

  return std::make_unique<Model>(device, modelBuilder);
}

void App::loadGameObjects() {
  std::shared_ptr<Model> gameObjectModel =
      Model::createModelFromFile(vkEngineDevice, "models/viking_room.obj");
  // std::shared_ptr<Model> gameObjectModel =
  // Model::createModelFromFile(vkEngineDevice, "models/smooth_vase.obj");
  // std::shared_ptr<Model> gameObjectModel =
  // createCubeModelIndexed(vkEngineDevice, glm::vec3{0.f, 0.f, 0.f});
  auto gObj = VkEngineGameObject::createGameObject();
  gObj.model = gameObjectModel;
  gObj.transform.translation = {.0f, .0f, 2.5f};
  gObj.transform.scale = glm::vec3{3.f};
  gObj.transform.rotation =
      glm::vec3{glm::half_pi<float>(), glm::half_pi<float>(), 0.f};

  // auto cube2 = VkEngineGameObject::createGameObject();
  // cube2.model = cubeModel;
  // cube2.transform.translation = {-.5f, .0f, .5f};
  // cube2.transform.scale = {.5f, .3f, .5f};
  // gameObjects.push_back(std::move(cube2));
  gameObjects.push_back(std::move(gObj));
}

} // namespace vkEngine