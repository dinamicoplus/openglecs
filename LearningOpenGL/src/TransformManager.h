#pragma once
#include "TransformComponent.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

class TransformManager {
public:

    static glm::mat4 getModelMatrix(const TransformComponent& transform) {
        if (transform.dirty) {
            // Orden estándar: Translate * Rotate * Scale
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.position);

            // Aplicar rotaciones en orden Z-Y-X (estándar)
            glm::mat4 rotationMatrix = glm::mat4(1.0f);
            rotationMatrix = glm::rotate(rotationMatrix, transform.rotation.z, glm::vec3(0, 0, 1)); // Z
            rotationMatrix = glm::rotate(rotationMatrix, transform.rotation.y, glm::vec3(0, 1, 0)); // Y
            rotationMatrix = glm::rotate(rotationMatrix, transform.rotation.x, glm::vec3(1, 0, 0)); // X

            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.scale);

            transform.cachedMatrix = translationMatrix * rotationMatrix * scaleMatrix;
            transform.dirty = false;
        }

        return transform.cachedMatrix;
    }
};

