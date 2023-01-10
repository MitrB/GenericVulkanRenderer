#include "model.hpp"
#include "device.hpp"
#include <cassert>
#include <cstring>
#include <iterator>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vkEngine {

Model::Model(VkEngineDevice &device, const std::vector<Vertex> &vertices)
    : vkEngineDevice(device) {
  createVertexBuffers(vertices);
}

Model::~Model() {
  vkDestroyBuffer(vkEngineDevice.device(), vertexBuffer, nullptr);
  vkFreeMemory(vkEngineDevice.device(), vertexBufferMemory, nullptr);
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
  auto vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "vertex count must be at least 3");
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

void Model::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}
void Model::draw(VkCommandBuffer commandBuffer) {
  vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
  std::vector<VkVertexInputBindingDescription> bindingDescription(1);
  bindingDescription[0].binding = 0;
  bindingDescription[0].stride = sizeof(Model::Vertex);
  bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescription;
  
}
std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
  std::vector<VkVertexInputAttributeDescription> attributedescription(1);
  attributedescription[0].binding = 0;
  attributedescription[0].location = 0;
  attributedescription[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributedescription[0].offset = 0;
  return attributedescription;
}

} // namespace vkEngine