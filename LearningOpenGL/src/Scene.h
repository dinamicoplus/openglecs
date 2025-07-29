#pragma once
#include <array>
#include <vector>
#include <bitset>
#include <memory>
#include <typeindex>
#include <unordered_map>

typedef unsigned long long EntityID;
const int MAX_COMPONENTS = 32;
const int MAX_ENTITIES = 1000;
typedef std::bitset<MAX_COMPONENTS> ComponentMask;

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
};

// Array de componentes genérico
template<typename T>
class ComponentArray : public IComponentArray {
    std::vector<T> m_Components;
    std::array<size_t, MAX_ENTITIES> m_EntityToIndexMap{};
    std::array<EntityID, MAX_ENTITIES> m_IndexToEntityMap{};
    size_t m_Size = 0;
public:
    void InsertData(EntityID entity, T component) {
        size_t newIndex = m_Size;
        m_EntityToIndexMap[entity] = newIndex;
        m_IndexToEntityMap[newIndex] = entity;
        if (newIndex >= m_Components.size())
            m_Components.push_back(std::move(component));
        else
            m_Components[newIndex] = std::move(component);
        m_Size++;
    }
    T* GetData(EntityID entity) {
        if (entity >= MAX_ENTITIES) return nullptr;
        return &m_Components[m_EntityToIndexMap[entity]];
    }

    // Agregar método para verificar si una entidad tiene este componente
    bool HasComponent(EntityID entity) const {
        if (entity >= MAX_ENTITIES) return false;
        size_t index = m_EntityToIndexMap[entity];
        return index < m_Size && m_IndexToEntityMap[index] == entity;
    }
    void RemoveData(EntityID entity) {
        size_t indexOfRemovedEntity = m_EntityToIndexMap[entity];
        size_t indexOfLastElement = m_Size - 1;
        m_Components[indexOfRemovedEntity] = m_Components[indexOfLastElement];
        EntityID entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
        m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;
        m_Size--;
    }
    std::vector<T>& GetComponents() { return m_Components; }
    const std::vector<T>& GetComponents() const { return m_Components; }
 
};

class Scene {
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> m_ComponentArrays;
    std::vector<EntityID> m_Entities;
    std::array<ComponentMask, MAX_ENTITIES> m_EntityMasks;
public:
    template<typename T>
    void RegisterComponentType() {
        std::type_index idx = std::type_index(typeid(T));
        if (m_ComponentArrays.find(idx) == m_ComponentArrays.end()) {
            m_ComponentArrays[idx] = std::make_unique<ComponentArray<T>>();
        }
    }

    template<typename... T>
    void RegisterComponentTypes() {
		(RegisterComponentType<T>(), ...);        
    }

    EntityID NewEntity() {
        EntityID newEntity = static_cast<EntityID>(m_Entities.size());
        m_Entities.push_back(newEntity);
        return newEntity;
    }

    template<typename T>
    ComponentArray<T>* GetComponentArray() {
        std::type_index idx = std::type_index(typeid(T));
        return static_cast<ComponentArray<T>*>(m_ComponentArrays[idx].get());
    }

    template<typename T>
    T* AssignComponent(EntityID entity) {
        auto* array = GetComponentArray<T>();
        array->InsertData(entity, T{}); // Inserta un componente por defecto
        return array->GetData(entity);  // Devuelve el puntero para modificarlo fuera
    }

    template<typename T>
    T* AssignComponent(EntityID entity, T component) {
        auto* array = static_cast<ComponentArray<T>*>(m_ComponentArrays[std::type_index(typeid(T))].get());
        array->InsertData(entity, std::move(component));
        // Aquí podrías usar un id de componente para el bitmask si lo necesitas
        return array->GetData(entity);
    }

    template<typename T>
    T* GetComponent(EntityID entity) {
        auto* array = static_cast<ComponentArray<T>*>(m_ComponentArrays[std::type_index(typeid(T))].get());
        return array->GetData(entity);
    }
};