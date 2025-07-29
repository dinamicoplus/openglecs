#pragma once
#include "TexturedModelComponent.h"
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
 #include <spdlog/spdlog.h>

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

    // Reemplaza 'String' por 'std::string' en la declaración del método
    static void readOBJfile(TexturedModelComponent& modelComponent, const char* filename)
	{
		FILE* file = fopen(filename, "r");
		if (!file) {
            // Reemplaza la línea problemática por esta:
            spdlog::error("Failed to open file: {}", filename);
			return;
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
    
        // Vector para acumular normales por vértice
        std::vector<glm::vec3> accumulatedNormals;
		unsigned int coordinateCount = 0;
		unsigned int normalCount = 0;
		unsigned int textureCoordinateCount = 0;

        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file)) {
			if (buffer[0] == 'v') {
				if (buffer[1] == ' ') {
					// Parsear posición del vértice
					Vertex vertex;
					sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
					vertex.r = vertex.g = vertex.b = 1.0f; // Color blanco por defecto
					vertex.u = vertex.v = vertex.s = vertex.t = vertex.w = 0.0f; // Coords textura por defecto
					vertices.push_back(vertex);
					coordinateCount++;

					// Inicializar vector de normales acumuladas
					accumulatedNormals.push_back(glm::vec3(0.0f));
				}
				else if (buffer[1] == 'n') {
					// Parsear normal del vértice
					if (normalCount > coordinateCount) {
						spdlog::error("Error: More normals than vertices in file: {}", filename);
						fclose(file);
						return;
					}
					glm::vec3 normal;
					sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
					vertices[normalCount].s = normal.x; // Asignar normal al vértice
					vertices[normalCount].t = normal.y;
					vertices[normalCount].w = normal.z;
					normalCount++;
				}
				else if (buffer[1] == 't') {
					// Parsear coordenadas de textura
					if (textureCoordinateCount > coordinateCount) {
						spdlog::error("Error: More texture coordinates than vertices in file: {}", filename);
						fclose(file);
						return;
					}
					float u, v;
					sscanf(buffer, "vt %f %f", &u, &v);
					vertices[textureCoordinateCount].u = u; // Asignar coordenadas de textura al vértice
					vertices[textureCoordinateCount].v = v;
					textureCoordinateCount++;
				}

			}
				
			
			else if (buffer[0] == 'f') {
                // Parsear índices de la cara - soportar múltiples formatos OBJ
                uint32_t idx1, idx2, idx3;
                uint32_t tex1, tex2, tex3;  // índices de textura (no usados pero necesarios para parsing)
                uint32_t norm1, norm2, norm3; // índices de normales (no usados pero necesarios para parsing)
                
                // Detectar el formato de la línea 'f'
                if (strchr(buffer, '/') == nullptr) {
                    // Formato: f v1 v2 v3
                    sscanf(buffer, "f %u %u %u", &idx1, &idx2, &idx3);
                } else {
                    // Contar las barras para determinar el formato
                    int slashCount = 0;
                    for (char* p = buffer; *p; p++) {
                        if (*p == '/') slashCount++;
                    }
                    
                    if (slashCount == 3) {
                        // Formato: f v1/vt1 v2/vt2 v3/vt3
                        sscanf(buffer, "f %u/%u %u/%u %u/%u", &idx1, &tex1, &idx2, &tex2, &idx3, &tex3);
                    } else if (slashCount == 6) {
                        // Formato: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                        sscanf(buffer, "f %u/%u/%u %u/%u/%u %u/%u/%u", 
                               &idx1, &tex1, &norm1, &idx2, &tex2, &norm2, &idx3, &tex3, &norm3);
                    }
                }
                
                indices.push_back(idx1 - 1); // Los índices OBJ son base 1
                indices.push_back(idx2 - 1);
                indices.push_back(idx3 - 1);
            }

        }

        // Calcular normales por cara
        for (size_t i = 0; i < indices.size(); i += 3) {
            // Obtener los índices del triángulo
            uint32_t idx1 = indices[i];
            uint32_t idx2 = indices[i + 1];
            uint32_t idx3 = indices[i + 2];
            
            // Obtener las posiciones de los vértices
            glm::vec3 v1(vertices[idx1].x, vertices[idx1].y, vertices[idx1].z);
            glm::vec3 v2(vertices[idx2].x, vertices[idx2].y, vertices[idx2].z);
            glm::vec3 v3(vertices[idx3].x, vertices[idx3].y, vertices[idx3].z);
            
            // Calcular vectores del triángulo
            glm::vec3 edge1 = v2 - v1;
            glm::vec3 edge2 = v3 - v1;
            glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
            
            // Acumular (no sobrescribir) las normales
            accumulatedNormals[idx1] += faceNormal;
            accumulatedNormals[idx2] += faceNormal;
            accumulatedNormals[idx3] += faceNormal;
        }

        // Normalizar las normales acumuladas
        for (size_t i = 0; i < vertices.size(); ++i) {
            glm::vec3 avgNormal = glm::normalize(accumulatedNormals[i]);
            vertices[i].s = avgNormal.x;
            vertices[i].t = avgNormal.y;
            vertices[i].w = avgNormal.z;
        }

        fclose(file);

		modelComponent.m_Vertices = std::move(vertices);
		modelComponent.m_Indices = std::move(indices);
		modelComponent.m_VertexCount = modelComponent.m_Vertices.size();
		modelComponent.m_IndexCount = modelComponent.m_Indices.size();
	}

};

