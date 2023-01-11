#include "model.hpp"

// std
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan_core.h>

namespace vkEngine {

Model::Model(VkEngineDevice &device, Model::Builder &builder)
    : vkEngineDevice{device} {
  createVertexBuffers(builder.vertices);
  createIndexBuffers(builder.indices);
}

Model::~Model() {
  vkDestroyBuffer(vkEngineDevice.device(), vertexBuffer, nullptr);
  vkFreeMemory(vkEngineDevice.device(), vertexBufferMemory, nullptr);
  if (hasIndexBuffer) {
    vkDestroyBuffer(vkEngineDevice.device(), indexBuffer, nullptr);
    vkFreeMemory(vkEngineDevice.device(), indexBufferMemory, nullptr);
  }
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
  vkEngineDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              vertexBuffer, vertexBufferMemory);

  void *data;
  vkMapMemory(vkEngineDevice.device(), vertexBufferMemory, 0, bufferSize, 0,
              &data);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(vkEngineDevice.device(), vertexBufferMemory);
}

void Model::createIndexBuffers(const std::vector<uint16_t> &indices) {
  indexCount = static_cast<uint16_t>(indices.size());
  hasIndexBuffer = indexCount > 0;

  if (!hasIndexBuffer) {
    return;
  }

  VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
  vkEngineDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              indexBuffer, indexBufferMemory);

  void *data;
  vkMapMemory(vkEngineDevice.device(), indexBufferMemory, 0, bufferSize, 0,
              &data);
  memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(vkEngineDevice.device(), indexBufferMemory);
}

void Model::draw(VkCommandBuffer commandBuffer) {
  if (hasIndexBuffer) {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  } else {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  }
}

void Model::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16); // change to 32 bit indices if models too big
  }
}

std::vector<VkVertexInputBindingDescription>
Model::Vertex::getBindingDescriptions() {
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription>
Model::Vertex::getAttributeDescriptions() {
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[0].offset = 0;

  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);
  return attributeDescriptions;
}

} // namespace vkEngine