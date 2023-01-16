#include "app.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "descriptors.hpp"
#include "device.hpp"
#include "frame_info.hpp"
#include "game_object.hpp"
#include "keyboard_movement_controller.hpp"
#include "model.hpp"
#include "swap_chain.hpp"
#include "systems/point_light_system.hpp"
#include "systems/simple_render_system.hpp"

// std
#include <array>
#include <chrono>
#include <cstdint>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>
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
  glm::mat4 projection{1.f};
  glm::mat4 view{1.f};
  glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
  glm::vec3 lightPosition{1.f};
  alignas(16) glm::vec4 lightColor{1.f};
};

App::App() {
  globalPool = VkEngineDescriptorPool::Builder(vkEngineDevice)
                   .setMaxSets(VkEngineSwapChain::MAX_FRAMES_IN_FLIGHT)
                   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                VkEngineSwapChain::MAX_FRAMES_IN_FLIGHT)
                   .build();
  loadGameObjects();
}

App::~App() {}

void App::run() {
  // for (VkEngineGameObject &object : gameObjects) {
  //   std::cout << object.getId() << std::endl;
  // }

  std::vector<std::unique_ptr<VkEngineBuffer>> uboBuffers(
      VkEngineSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] =
        std::make_unique<VkEngineBuffer>(vkEngineDevice, sizeof(GlobalUbo), 1,
                                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uboBuffers[i]->map();
  }

  auto globalSetLayout = VkEngineDescriptorSetLayout::Builder(vkEngineDevice)
                             .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         VK_SHADER_STAGE_ALL_GRAPHICS)
                             .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(
      VkEngineSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    VkEngineDescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
        .build(globalDescriptorSets[i]);
  }

  SimpleRenderSystem simpleRenderSystem(
      vkEngineDevice, vkEngineRenderer.getSwapChainrenderPass(),
      globalSetLayout->getDescriptorSetLayout());

  PointLightSystem pointLightSystem(
      vkEngineDevice, vkEngineRenderer.getSwapChainrenderPass(),
      globalSetLayout->getDescriptorSetLayout());

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

    // calc framerate
    calculateFrameRate(delta);

    // delta = glm::min(delta, MAX_FRAME_TIME);

    cameraController.moveInPlaneXZ(window.getGLFWwindow(), delta, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    float aspect = vkEngineRenderer.getAspectRatio();
    // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
    camera.setPerspectiveProjection(glm::radians(70.f), aspect, .1f, 100.f);

    if (auto commandBuffer = vkEngineRenderer.beginFrame()) {
      int frameIndex = vkEngineRenderer.getFrameIndex();
      FrameInfo frameInfo{frameIndex,
                          delta,
                          commandBuffer,
                          camera,
                          globalDescriptorSets[frameIndex],
                          gameObjects};

      // update
      GlobalUbo ubo{};
      ubo.projection = camera.getProjection();
      ubo.view = camera.getView();
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      // render
      vkEngineRenderer.beginSwapChainrenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(frameInfo);
      pointLightSystem.render(frameInfo);
      vkEngineRenderer.endSwapChainrenderPass(commandBuffer);
      vkEngineRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(vkEngineDevice.device());
}

void App::loadGameObjects() {
  std::shared_ptr<Model> gameObjectModel =
      Model::createModelFromFile(vkEngineDevice, "models/viking_room.obj");
  std::shared_ptr<Model> quadModel =
      Model::createModelFromFile(vkEngineDevice, "models/quad.obj");

  auto gObj = VkEngineGameObject::createGameObject();
  gObj.model = gameObjectModel;
  gObj.transform.translation = {.0f, 2.0f, 2.f};
  gObj.transform.scale = glm::vec3{3.f};
  gObj.transform.rotation =
      glm::vec3{glm::half_pi<float>(), glm::half_pi<float>(), 0.f};
  gameObjects.emplace(gObj.getId(), std::move(gObj));

  gObj = VkEngineGameObject::createGameObject();
  gObj.model = quadModel;
  gObj.transform.translation = {.0f, 2.0f, 0.f};
  gObj.transform.scale = glm::vec3{3.f};
  // gObj.transform.rotation = glm::vec3{glm::half_pi<float>(),
  // glm::half_pi<float>(), 0.f};
  gObj.transform.rotation = glm::vec3{.0f};
  gameObjects.emplace(gObj.getId(), std::move(gObj));
}

void App::calculateFrameRate(float delta) {
  numFrames++;
  timePassed += delta;
  if (timePassed >= 1) {
    int framerate = numFrames / timePassed;
    std::stringstream title;
    title << "Running at " << framerate << " fps.";
    glfwSetWindowTitle(window.getGLFWwindow(), title.str().c_str());
    numFrames = 0;
    timePassed -= 1;
    // frameTime = float(1000.0 / framerate);
  }
}

} // namespace vkEngine