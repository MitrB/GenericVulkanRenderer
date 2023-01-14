#pragma once

#include "device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace vkEngine {

class VkEngineDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(VkEngineDevice &vkEngineDevice) : vkEngineDevice{vkEngineDevice} {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<VkEngineDescriptorSetLayout> build() const;

   private:
    VkEngineDevice &vkEngineDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  VkEngineDescriptorSetLayout(
      VkEngineDevice &vkEngineDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~VkEngineDescriptorSetLayout();
  VkEngineDescriptorSetLayout(const VkEngineDescriptorSetLayout &) = delete;
  VkEngineDescriptorSetLayout &operator=(const VkEngineDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

 private:
  VkEngineDevice &vkEngineDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class VkEngineDescriptorWriter;
};

class VkEngineDescriptorPool {
 public:
  class Builder {
   public:
    Builder(VkEngineDevice &vkEngineDevice) : vkEngineDevice{vkEngineDevice} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<VkEngineDescriptorPool> build() const;

   private:
    VkEngineDevice &vkEngineDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  VkEngineDescriptorPool(
      VkEngineDevice &vkEngineDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~VkEngineDescriptorPool();
  VkEngineDescriptorPool(const VkEngineDescriptorPool &) = delete;
  VkEngineDescriptorPool &operator=(const VkEngineDescriptorPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

 private:
  VkEngineDevice &vkEngineDevice;
  VkDescriptorPool descriptorPool;

  friend class VkEngineDescriptorWriter;
};

class VkEngineDescriptorWriter {
 public:
  VkEngineDescriptorWriter(VkEngineDescriptorSetLayout &setLayout, VkEngineDescriptorPool &pool);

  VkEngineDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  VkEngineDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  VkEngineDescriptorSetLayout &setLayout;
  VkEngineDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace vkEngine