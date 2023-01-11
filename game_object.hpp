#pragma once

#include "model.hpp"

// libs
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>

namespace vkEngine {

struct TransformComponent {
  glm::vec3 translation;
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{};

  // implement Quaternions?
  glm::mat4 mat4() {
    // slower

    // auto transform = glm::translate(glm::mat4(1.f), translation);
    // rotation matrix

    // transform = glm::rotate(transform, rotation.y, {0.f, 1.f, 0.f});
    // transform = glm::rotate(transform, rotation.x, {1.f, 0.f, 0.f});
    // transform = glm::rotate(transform, rotation.z, {0.f, 0.f, 1.f});
    // transform = glm::scale(transform, scale);
    // return transform;

    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    return glm::mat4{{
                         scale.x * (c1 * c3 + s1 * s2 * s3),
                         scale.x * (c2 * s3),
                         scale.x * (c1 * s2 * s3 - c3 * s1),
                         0.0f,
                     },
                     {
                         scale.y * (c3 * s1 * s2 - c1 * s3),
                         scale.y * (c2 * c3),
                         scale.y * (c1 * c3 * s2 + s1 * s3),
                         0.0f,
                     },
                     {
                         scale.z * (c2 * s1),
                         scale.z * (-s2),
                         scale.z * (c1 * c2),
                         0.0f,
                     },
                     {translation.x, translation.y, translation.z, 1.0f}};
  }
};

class VkEngineGameObject {
public:
  using id_t = unsigned int;

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