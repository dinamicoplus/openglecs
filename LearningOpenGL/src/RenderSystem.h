#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "TexturedModelComponent.h"
#include "Shader.h"

class RenderSystem
{
public:
    static void render(const TexturedModelComponent* model, const Shader& shader) {
        shader.use();
        shader.setMatrix4("model", model->m_ModelMatrix);
        glBindVertexArray(model->m_VAO);
        if (model->m_TextureID != 0)
        {
            shader.setBool("hasTexture", true);
            glBindTexture(GL_TEXTURE_2D, model->m_TextureID);
        }
        else
        {
            shader.setBool("hasTexture", false);
        }
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(model->m_Vertices.size()));
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }
};