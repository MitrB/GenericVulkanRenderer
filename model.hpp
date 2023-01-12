#pragma once

#include "device.hpp"
#include "buffer.hpp"

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

    bool operator==(const Vertex &other) const {
      return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
    }
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

  std::unique_ptr<VkEngineBuffer> vertexBuffer;
  uint32_t vertexCount;

  bool hasIndexBuffer = false;
  std::unique_ptr<VkEngineBuffer> indexBuffer;
  uint32_t indexCount;
};

} // namespace vkEngine
