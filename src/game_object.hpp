#pragma once

#include "model.hpp"

// libs
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace vkEngine {

struct TransformComponent {
  glm::vec3 translation;
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{};

  // implement Quaternions?
  glm::mat4 mat4();

  glm::mat3 normalMatrix();
};

class VkEngineGameObject {
public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, VkEngineGameObject>;

  static VkEngineGameObject createGameObject() {
    static id_t currentId = 0;
    return VkEngineGameObject{currentId++};
  };

  id_t getId() { return id; }

  std::shared_ptr<Model> model;
  glm::vec3 color{};
  TransformComponent transform{};

  VkEngineGameObject(const VkEngineGameObject &) = delete;
  VkEngineGameObject &operator=(const VkEngineGameObject &) = delete;
  VkEngineGameObject(VkEngineGameObject &&) = default;
  VkEngineGameObject &operator=(VkEngineGameObject &&) = default;

private:
  VkEngineGameObject(id_t objId) : id{objId} {};

  id_t id;
};
} // namespace vkEngine