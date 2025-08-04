#pragma once
#include "TexturedModelComponent.h"
//#include "Scene.h"
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

		// Cargar �ndices
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

    // Reemplaza 'String' por 'std::string' en la declaraci�n del m�todo
    static void readOBJfile(TexturedModelComponent& modelComponent, const char* filename)
	{
		FILE* file = fopen(filename, "r");
		if (!file) {
            // Reemplaza la l�nea problem�tica por esta:
            spdlog::error("Failed to open file: {}", filename);
			return;
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
    
        // Vector para acumular normales por v�rtice
        std::vector<glm::vec3> accumulatedNormals;
		unsigned int coordinateCount = 0;
		unsigned int normalCount = 0;
		unsigned int textureCoordinateCount = 0;

        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file)) {
			if (buffer[0] == 'v') {
				if (buffer[1] == ' ') {
					// Parsear posici�n del v�rtice
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
					// Parsear normal del v�rtice
					if (normalCount > coordinateCount) {
						spdlog::error("Error: More normals than vertices in file: {}", filename);
						fclose(file);
						return;
					}
					glm::vec3 normal;
					sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
					vertices[normalCount].s = normal.x; // Asignar normal al v�rtice
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
					vertices[textureCoordinateCount].u = u; // Asignar coordenadas de textura al v�rtice
					vertices[textureCoordinateCount].v = v;
					textureCoordinateCount++;
				}

			}
				
			
			else if (buffer[0] == 'f') {
                // Parsear �ndices de la cara - soportar m�ltiples formatos OBJ
                uint32_t idx1, idx2, idx3;
                uint32_t tex1, tex2, tex3;  // �ndices de textura (no usados pero necesarios para parsing)
                uint32_t norm1, norm2, norm3; // �ndices de normales (no usados pero necesarios para parsing)
                
                // Detectar el formato de la l�nea 'f'
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
                
                indices.push_back(idx1 - 1); // Los �ndices OBJ son base 1
                indices.push_back(idx2 - 1);
                indices.push_back(idx3 - 1);
            }

        }

        fclose(file);

		modelComponent.m_Vertices = std::move(vertices);
		modelComponent.m_Indices = std::move(indices);
		modelComponent.m_VertexCount = modelComponent.m_Vertices.size();
		modelComponent.m_IndexCount = modelComponent.m_Indices.size();
	}

	static void calculateNormals(TexturedModelComponent& modelComponent) {
		if (modelComponent.m_Indices.empty() || modelComponent.m_Vertices.empty()) {
			spdlog::error("Cannot calculate normals: Model has no indices or vertices.");
			return;
		}
		std::vector<glm::vec3> normals(modelComponent.m_VertexCount, glm::vec3(0.0f));
		for (size_t i = 0; i < modelComponent.m_IndexCount; i += 3) {
			uint32_t idx1 = modelComponent.m_Indices[i];
			uint32_t idx2 = modelComponent.m_Indices[i + 1];
			uint32_t idx3 = modelComponent.m_Indices[i + 2];
			glm::vec3 v1(modelComponent.m_Vertices[idx1].x, modelComponent.m_Vertices[idx1].y, modelComponent.m_Vertices[idx1].z);
			glm::vec3 v2(modelComponent.m_Vertices[idx2].x, modelComponent.m_Vertices[idx2].y, modelComponent.m_Vertices[idx2].z);
			glm::vec3 v3(modelComponent.m_Vertices[idx3].x, modelComponent.m_Vertices[idx3].y, modelComponent.m_Vertices[idx3].z);
			glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
			normals[idx1] += normal;
			normals[idx2] += normal;
			normals[idx3] += normal;
		}
		for (size_t i = 0; i < modelComponent.m_VertexCount; ++i) {
			normals[i] = glm::normalize(normals[i]);
			modelComponent.m_Vertices[i].s = normals[i].x;
			modelComponent.m_Vertices[i].t = normals[i].y;
			modelComponent.m_Vertices[i].w = normals[i].z;
		}
	}

};

