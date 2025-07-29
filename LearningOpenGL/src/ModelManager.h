#pragma once
#include "TexturedModelComponent.h"
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class ModelManager
{
	public:
	static void initModel(TexturedModelComponent& modelComponent) {
		modelComponent.m_Rotation = glm::vec3(0.0f);
		modelComponent.m_Scale = glm::vec3(1.0f);
		modelComponent.m_ModelMatrix = glm::mat4(1.0f);
	}

	static void initModelIntoGPU(TexturedModelComponent& modelComponent) {
		glGenVertexArrays(1, &modelComponent.m_VAO);
		glGenBuffers(1, &modelComponent.m_VBO);
		glGenBuffers(1, &modelComponent.m_EBO); // <-- Agregar EBO
	}

	static void deleteModelFromGPU(TexturedModelComponent& modelComponent) {
		glDeleteVertexArrays(1, &modelComponent.m_VAO);
		glDeleteBuffers(1, &modelComponent.m_VBO);
		glDeleteBuffers(1, &modelComponent.m_EBO); // <-- Eliminar EBO
	}

	static void loadModelIntoGPU(TexturedModelComponent& modelComponent, bool removeFromMem = false) {
		glBindVertexArray(modelComponent.m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, modelComponent.m_VBO);
		glBufferData(GL_ARRAY_BUFFER, 
			modelComponent.m_Vertices.size()*sizeof(Vertex),
			modelComponent.m_Vertices.data(), 
			GL_STATIC_DRAW);

		// Cargar índices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelComponent.m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			modelComponent.m_Indices.size() * sizeof(uint32_t),
			modelComponent.m_Indices.data(),
			GL_STATIC_DRAW);

		// Configurar atributos (igual que antes)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		if (removeFromMem) {
			modelComponent.m_Vertices.clear();
			modelComponent.m_Vertices.shrink_to_fit();
			modelComponent.m_Indices.clear();
			modelComponent.m_Indices.shrink_to_fit();
		}
	}

	static void loadDataIntoModel(TexturedModelComponent& modelComponent, 
		std::vector<Vertex> vertices,
		std::vector<uint32_t> indices = std::vector<uint32_t>(0)
	)
	{
		modelComponent.m_Vertices = std::move(vertices);
		modelComponent.m_Indices = std::move(indices);
		modelComponent.m_VertexCount = modelComponent.m_Vertices.size();
		modelComponent.m_IndexCount = modelComponent.m_Indices.size();
	}

	static std::vector<Vertex> floatArrayToVector(float *vertices, size_t size)
	{
		std::vector<Vertex> vertexVector;
		for (size_t i = 0; i < size; i += 11) {
			Vertex vertex;
			vertex.x = vertices[i];
			vertex.y = vertices[i + 1];
			vertex.z = vertices[i + 2];
			vertex.r = vertices[i + 3];
			vertex.g = vertices[i + 4];
			vertex.b = vertices[i + 5];
			vertex.u = vertices[i + 6];
			vertex.v = vertices[i + 7];
			vertex.s = vertices[i + 8];
			vertex.t = vertices[i + 9];
			vertex.w = vertices[i + 10];
			vertexVector.push_back(vertex);
		}
		return vertexVector;
	}

};

