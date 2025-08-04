#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <memory>
#include <typeindex>
#include <bitset>
#include "TransformComponent.h"
#include "TexturedModelComponent.h"

typedef unsigned long long EntityID;
const int MAX_COMPONENTS = 32;
const int MAX_ENTITIES = 1000;
typedef std::bitset<MAX_COMPONENTS> ComponentMask;


static const std::unordered_map<std::type_index, size_t> ComponentType {
    { std::type_index(typeid(TransformComponent)), 0 },
    { std::type_index(typeid(TexturedModelComponent)), 1}
};

// Array de componentes gen�rico
template<typename T>
class ComponentArray {
public:
	std::type_index m_TypeIndex = std::type_index(typeid(T));
    std::array<T, MAX_ENTITIES> m_Components;
    std::unordered_map<size_t, EntityID> m_EntityToIndexMap{};
    std::unordered_map<EntityID, size_t> m_IndexToEntityMap{};
    size_t m_Size = 0;
};

struct Scene {
    ComponentArray<TransformComponent> TransformComponentArray = ComponentArray<TransformComponent>();
    ComponentArray<TexturedModelComponent> TexturedModelComponentArray = ComponentArray<TexturedModelComponent>();
    std::array<EntityID, MAX_ENTITIES> m_Entities{ (unsigned long long)-1 }; // Current Entities in the scene
    std::array<ComponentMask, MAX_ENTITIES> m_EntityMasks;
	std::unordered_map<ComponentMask, std::unordered_set<unsigned long long>> m_ComponentMaskToIdxMap;
    unsigned long long entityIdx = 0;
};


class ComponentArrayManager {
public:
    template<typename T>
    static void ReserveComponentForEntity(ComponentArray<T>* componentArray, EntityID entity) {
        // Verificar si ya existe un �ndice para esta entidad
        auto it = componentArray->m_IndexToEntityMap.find(entity);
        if (it != componentArray->m_IndexToEntityMap.end()) return; // Ya existe
        
        size_t newIndex = componentArray->m_Size;
        componentArray->m_IndexToEntityMap[entity] = newIndex;
        componentArray->m_EntityToIndexMap[newIndex] = entity;
        componentArray->m_Size++;
    }

    template<typename T>
    static T* AddComponentToEntity(ComponentArray<T>* componentArray, EntityID entity, T component) {
        ReserveComponentForEntity(componentArray, entity);
        
        // Obtener �ndice actual para esta entidad
        auto it = componentArray->m_IndexToEntityMap.find(entity);
        if (it == componentArray->m_IndexToEntityMap.end()) return nullptr;
        
        size_t index = it->second;
        componentArray->m_Components[index] = std::move(component); // Asignar directamente al array
        
        return &componentArray->m_Components[index]; // Devolver puntero al componente almacenado
    }

    template<typename T>
    static T* GetComponent(ComponentArray<T>* componentArray, EntityID entity) {
        // Verificar si existe un componente para esta entidad
        auto it = componentArray->m_IndexToEntityMap.find(entity);
        if (it == componentArray->m_IndexToEntityMap.end()) return nullptr;
        
        size_t index = it->second;
        return &componentArray->m_Components[index];
    }

    template<typename T>
    static std::array<T, MAX_ENTITIES>& GetComponents(ComponentArray<T>* componentArray) {
        return componentArray->m_Components;
    }

    template<typename T>
    static void RemoveComponent(ComponentArray<T>* componentArray, EntityID entity) {
        // Verificar si existe un componente para esta entidad
        auto entityIt = componentArray->m_IndexToEntityMap.find(entity);
        if (entityIt == componentArray->m_IndexToEntityMap.end()) return;
        
        size_t indexOfRemovedEntity = entityIt->second;
        size_t indexOfLastElement = componentArray->m_Size - 1;
        
        // Si no es el �ltimo elemento, mover el �ltimo a su posici�n
        if (indexOfRemovedEntity != indexOfLastElement) {
            componentArray->m_Components[indexOfRemovedEntity] = std::move(componentArray->m_Components[indexOfLastElement]);
            
            // Actualizar mapas para la entidad movida
            EntityID entityOfLastElement = componentArray->m_EntityToIndexMap[indexOfLastElement];
            componentArray->m_IndexToEntityMap[entityOfLastElement] = indexOfRemovedEntity;
            componentArray->m_EntityToIndexMap[indexOfRemovedEntity] = entityOfLastElement;
        }
        
        // Eliminar las entradas de los mapas
        componentArray->m_IndexToEntityMap.erase(entity);
        componentArray->m_EntityToIndexMap.erase(indexOfLastElement);
        componentArray->m_Size--;
    }
};

class SceneManager {
public: // Cambiar a public para facilitar el uso
    static EntityID NewEntity(Scene* scene) {
        if (scene->entityIdx >= MAX_ENTITIES) return static_cast<EntityID>(-1); // Verificaci�n de l�mite
        
        EntityID newEntity = scene->entityIdx; // Simplificar: usar directamente el �ndice como EntityID
        scene->m_Entities[newEntity] = newEntity;
        ComponentMask emptyMask; // Crear m�scara vac�a
        scene->m_EntityMasks[newEntity] = emptyMask;
        scene->m_ComponentMaskToIdxMap[emptyMask].insert(newEntity);
        scene->entityIdx++;
        return newEntity;
    }

    template<typename T>
    static ComponentArray<T>* GetComponentArray(Scene* scene) {
        if constexpr (std::is_same_v<T, TransformComponent>) {
            return &scene->TransformComponentArray;
        } else if constexpr (std::is_same_v<T, TexturedModelComponent>) {
            return &scene->TexturedModelComponentArray;
        }
        return nullptr; // A�adir caso por defecto
    }

    template<typename T>
    static T* AssignComponent(Scene* scene, EntityID entity, T component) {
        if (entity >= MAX_ENTITIES) return nullptr; // Verificaci�n de l�mite
        
        // Obtener componente array
        ComponentArray<T>* array = GetComponentArray<T>(scene);
        if (!array) return nullptr;
        
        // Guardar m�scara anterior para actualizar el mapa
        ComponentMask oldMask = scene->m_EntityMasks[entity];
        
        // Asignar componente
        T* result = ComponentArrayManager::AddComponentToEntity<T>(array, entity, std::move(component));
        
        // Actualizar m�scara y mapa de componentes
        size_t componentTypeID = ComponentType.at(std::type_index(typeid(T)));
        scene->m_ComponentMaskToIdxMap[oldMask].erase(entity); // Quitar entidad de la m�scara anterior
        scene->m_EntityMasks[entity].set(componentTypeID); // Actualizar m�scara
        scene->m_ComponentMaskToIdxMap[scene->m_EntityMasks[entity]].insert(entity); // Agregar a nueva m�scara
        
        return result;
    }

    template<typename T>
    static T* AssignComponent(Scene* scene, EntityID entity) {
        return AssignComponent(scene, entity, T{}); // Reutilizar l�gica existente
    }

    template<typename T>
    static T* GetComponent(Scene* scene, EntityID entity) {
        if (entity >= MAX_ENTITIES) return nullptr; // Verificaci�n de l�mite
        
        ComponentArray<T>* array = GetComponentArray<T>(scene);
        if (!array) return nullptr;
        
        return ComponentArrayManager::GetComponent<T>(array, entity);
    }

    template<typename T>
    static void RemoveComponent(Scene* scene, EntityID entity) {
        if (entity >= MAX_ENTITIES) return; // Verificaci�n de l�mite
        
        ComponentArray<T>* array = GetComponentArray<T>(scene);
        if (!array) return;
        
        // Guardar m�scara anterior
        ComponentMask oldMask = scene->m_EntityMasks[entity];
        
        // Remover componente
        ComponentArrayManager::RemoveComponent<T>(array, entity);
        
        // Actualizar m�scara y mapa
        size_t componentTypeID = ComponentType.at(std::type_index(typeid(T)));
        scene->m_ComponentMaskToIdxMap[oldMask].erase(entity);
        scene->m_EntityMasks[entity].reset(componentTypeID);
        scene->m_ComponentMaskToIdxMap[scene->m_EntityMasks[entity]].insert(entity);
    }

    static void ClearAllEntityComponents(Scene *scene, EntityID entity) {
        for (auto& componentType : ComponentType) {
            if (componentType.first == typeid(TransformComponent)) {
                RemoveComponent<TransformComponent>(scene, entity);
            }
            else if (componentType.first == typeid(TexturedModelComponent)) {
                RemoveComponent<TexturedModelComponent>(scene, entity);
            }
        }
    }

    static void DeleteEntity(Scene* scene, EntityID entity) {
        if (entity >= MAX_ENTITIES || entity >= scene->entityIdx) return;
        
        // Eliminar de la colecci�n por m�scara
        scene->m_ComponentMaskToIdxMap[scene->m_EntityMasks[entity]].erase(entity);
        
        // Eliminar componentes
        ClearAllEntityComponents(scene, entity);
        
        // Si no es la �ltima entidad, mover la �ltima a su posici�n
        EntityID lastEntity = scene->entityIdx - 1;
        if (entity != lastEntity) {
            scene->m_Entities[entity] = scene->m_Entities[lastEntity];
            scene->m_EntityMasks[entity] = scene->m_EntityMasks[lastEntity];
            
            // Actualizar mapa de m�scaras para la entidad movida
            scene->m_ComponentMaskToIdxMap[scene->m_EntityMasks[entity]].erase(lastEntity);
            scene->m_ComponentMaskToIdxMap[scene->m_EntityMasks[entity]].insert(entity);
        }
        
        // Limpiar �ltima posici�n
        scene->m_Entities[lastEntity] = static_cast<EntityID>(-1);
        scene->m_EntityMasks[lastEntity].reset();
        scene->entityIdx--;
    }

	template<typename T>
    static ComponentArray<T> *GetComponentArrayByType(Scene* scene) {
		std::type_index typeIndex = std::type_index(typeid(T));
        if (typeIndex == std::type_index(typeid(TransformComponent))) {
            return &scene->TransformComponentArray;
        } else if (typeIndex == std::type_index(typeid(TexturedModelComponent))) {
            return &scene->TexturedModelComponentArray;
        }
        return nullptr; // A�adir caso por defecto
	}


    template<typename T>
    static std::vector<std::pair<EntityID, T*>> FilterComponentsByMask(Scene* scene, const ComponentMask& targetMask) {
        std::vector<std::pair<EntityID, T*>> result;

        for (const auto& [mask, entities] : scene->m_ComponentMaskToIdxMap) {
            // Verificar si esta m�scara incluye todos los bits de targetMask
            if ((mask & targetMask) == targetMask) {
                ComponentArray<T>* array = GetComponentArray<T>(scene);
                if (!array) continue;

                for (EntityID entity : entities) {
                    if (T* component = ComponentArrayManager::GetComponent(array, entity)) {
                        result.emplace_back(entity, component);
                    }
                }
            }
        }

        return result;
    }
};
