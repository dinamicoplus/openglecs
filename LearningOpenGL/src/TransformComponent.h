#pragma once
#include <GLM/glm.hpp>

struct TransformComponent {
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f };
    glm::vec3 scale{ 1.0f };
    mutable bool dirty = true;
    mutable glm::mat4 cachedMatrix{ 1.0f };
};