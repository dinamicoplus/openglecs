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


    static void readOBJfile(TexturedModelComponent& modelComponent, const char* filename)
	{
		FILE* file = fopen(filename, "r");
		if (!file) {
            // Reemplaza la l�nea problem�tica por esta:
            spdlog::error("Failed to open file: {}", filename);
			return;
		}

		struct Key3 {
			uint16_t pos, tex, nor;  // 16-bit unsigned integers (more than enough)

			bool operator==(const Key3& other) const {
				return pos == other.pos && tex == other.tex && nor == other.nor;
			}
		};

		struct Key3Hash {
			std::size_t operator()(const Key3& k) const {
				// Pack all three into a 64-bit integer:
				uint64_t combined = (uint64_t(k.pos) << 30) | (uint64_t(k.tex) << 15) | uint64_t(k.nor);
				return std::hash<uint64_t>{}(combined);
			}
		};

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		std::vector<glm::vec3> verticesPosition;
		std::vector<glm::vec2> verticesTexture;
		std::vector<glm::vec3> verticesNormals;
		std::unordered_map<Key3, uint32_t, Key3Hash> vertices_map;

		unsigned int coordinateCount = 0;
		unsigned int normalCount = 0;
		unsigned int textureCoordinateCount = 0;

        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file)) {
			if (buffer[0] == 'v') {
				if (buffer[1] == ' ') {
					glm::vec3 vertex;
					sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
					verticesPosition.push_back({ vertex.x, vertex.y, vertex.z });
					coordinateCount++;
				}
				else if (buffer[1] == 'n') {
					glm::vec3 normal;
					sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
					verticesNormals.push_back(normal);
					normalCount++;
				}
				else if (buffer[1] == 't') {
					// Parsear coordenadas de textura
					glm::vec2 tex;
					sscanf(buffer, "vt %f %f", &tex.x, &tex.y);
					verticesTexture.push_back(tex);
					textureCoordinateCount++;
				}

			}
				
			
			else if (buffer[0] == 'f') {
                // Parsear �ndices de la cara - soportar m�ltiples formatos OBJ
                uint32_t idx1, idx2, idx3;
                uint32_t tex1, tex2, tex3;  
                uint32_t norm1, norm2, norm3; 

                
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

#define IND_FORMAT(a) ((a) > 0 ? static_cast<uint16_t>((a) - 1) : UINT16_MAX)

				Key3 vertex1{ IND_FORMAT(idx1), IND_FORMAT(tex1), IND_FORMAT(norm1) };
				Key3 vertex2{ IND_FORMAT(idx2), IND_FORMAT(tex2), IND_FORMAT(norm2) };
				Key3 vertex3{ IND_FORMAT(idx3), IND_FORMAT(tex3), IND_FORMAT(norm3) };
                
				auto process_vertex = [&](const Key3& key) -> uint32_t {
					auto it = vertices_map.find(key);
					if (it != vertices_map.end()) {
						return it->second;  // existing vertex index
					}
					// Construct Vertex from key:
					Vertex v{};
					v.x = verticesPosition[key.pos].x;
					v.y = verticesPosition[key.pos].y;
					v.z = verticesPosition[key.pos].z;

					v.s = verticesNormals[key.nor].x;
					v.t = verticesNormals[key.nor].y;
					v.w = verticesNormals[key.nor].z;

					v.u = verticesTexture[key.tex].x;
					v.v = verticesTexture[key.tex].y;

					uint32_t index = (uint32_t)vertices.size();
					vertices.push_back(v);
					vertices_map[key] = index;
					return index;
					};

				// Then for each vertex in face:
				uint32_t i1 = process_vertex(vertex1);
				uint32_t i2 = process_vertex(vertex2);
				uint32_t i3 = process_vertex(vertex3);

				indices.push_back(i1);
				indices.push_back(i2);
				indices.push_back(i3);

            }

        }

        fclose(file);

		modelComponent.m_Vertices = std::move(vertices);
		modelComponent.m_Indices = std::move(indices);
		modelComponent.m_VertexCount = modelComponent.m_Vertices.size();
		modelComponent.m_IndexCount = modelComponent.m_Indices.size();
		spdlog::info("Loaded OBJ: {} vertices, {} indices", modelComponent.m_VertexCount, modelComponent.m_IndexCount);

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

	static void saveMBFFile(const TexturedModelComponent& modelComponent, const char* filename) {
		FILE* file = fopen(filename, "wb");
		if (!file) {
			spdlog::error("Failed to open file for writing: {}", filename);
			return;
		}
		uint32_t vertexCount = static_cast<uint32_t>(modelComponent.m_Vertices.size());
		uint32_t indexCount = static_cast<uint32_t>(modelComponent.m_Indices.size());
		fwrite(&vertexCount, sizeof(uint32_t), 1, file);
		fwrite(&indexCount, sizeof(uint32_t), 1, file);
		fwrite(modelComponent.m_Vertices.data(), sizeof(Vertex), vertexCount, file);
		fwrite(modelComponent.m_Indices.data(), sizeof(uint32_t), indexCount, file);
		fclose(file);
		spdlog::info("Saved MBF file: {} with {} vertices and {} indices", filename, vertexCount, indexCount);
	}

	static void loadMBFFile(TexturedModelComponent& modelComponent, const char* filename) {
		FILE* file = fopen(filename, "rb");
		if (!file) {
			spdlog::error("Failed to open MBF file: {}", filename);
			return;
		}
		uint32_t vertexCount, indexCount;
		fread(&vertexCount, sizeof(uint32_t), 1, file);
		fread(&indexCount, sizeof(uint32_t), 1, file);
		modelComponent.m_Vertices.resize(vertexCount);
		modelComponent.m_Indices.resize(indexCount);
		fread(modelComponent.m_Vertices.data(), sizeof(Vertex), vertexCount, file);
		fread(modelComponent.m_Indices.data(), sizeof(uint32_t), indexCount, file);
		fclose(file);
		modelComponent.m_VertexCount = vertexCount;
		modelComponent.m_IndexCount = indexCount;
		spdlog::info("Loaded MBF file: {} with {} vertices and {} indices", filename, vertexCount, indexCount);
	}

};

