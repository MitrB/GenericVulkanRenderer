#pragma once

#include "model.hpp"
#include <glm/fwd.hpp>
#include <memory>

namespace vkEngine {

struct Transform2dComponent {
    glm::vec2 translation;
    glm::mat2 mat2() {return glm::mat2{1.f};}
};

class vkEngineGameObject {
    public:
    using id_t = unsigned int;

    static vkEngineGameObject createGameObject() {
        static id_t currentId = 0;
        return vkEngineGameObject{currentId++};
    };

    id_t getId() {return id;}

    std::shared_ptr<Model> model;
    glm::vec3 color{};
    Transform2dComponent transform2d{};

    vkEngineGameObject(const vkEngineGameObject &) = delete;
    vkEngineGameObject &operator=(const vkEngineGameObject &) = delete;
    vkEngineGameObject(vkEngineGameObject&&) = default;
    vkEngineGameObject &operator=(vkEngineGameObject &&) = default;

    private:
    vkEngineGameObject(id_t objId) : id{objId} {};

    id_t id;
};
} // namespace vkEngine