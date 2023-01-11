#pragma once

#include "device.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vkEngine {

class Model {
public:

  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal{};
    glm::vec2 uv{};

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
  };

  struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<uint16_t> indices{};

    void loadModel(const std::string &filepath);
  };

  Model(VkEngineDevice &device, Model::Builder & builder);
  ~Model();

  Model(const Model &) = delete;
  Model &operator=(const Model &) = delete;

  static std::unique_ptr<Model> createModelFromFile(VkEngineDevice &device, const std::string &filepath); 

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

private:
  void createVertexBuffers(const std::vector<Vertex> &vertices);
  void createIndexBuffers(const std::vector<uint16_t> &indices);

  VkEngineDevice &vkEngineDevice;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
  uint32_t indexCount;
};

} // namespace vkEngine
