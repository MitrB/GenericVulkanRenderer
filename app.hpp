#pragma once

#include "device.hpp"
#include "model.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include "game_object.hpp"
#include "descriptors.hpp"

// std
#include <chrono>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vkEngine {
class App {
public:
  static constexpr int WIDTH = 600;
  static constexpr int HEIGHT = 600;

  App();
  ~App();

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  void run();

private:
  void calculateFrameRate(float delta); 
  float timePassed = 0;
  int numFrames = 0;

  float MAX_FRAME_TIME = 0.1f;
  void loadGameObjects();

  Window window{WIDTH, HEIGHT, "Vulkan Engine"};
  VkEngineDevice vkEngineDevice{window};
  VkEngineRenderer vkEngineRenderer{window, vkEngineDevice};

  // order of declerations matter
  std::unique_ptr<VkEngineDescriptorPool> globalPool{};
  VkEngineGameObject::Map gameObjects;
};

} // namespace vkEngine