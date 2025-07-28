#pragma once

#include <cstdint>

struct Entity
{
    using IdType = std::uint32_t;
    IdType id{0};

    // Operadores útiles para comparación y conversión
    bool operator==(const Entity& other) const { return id == other.id; }
    bool operator!=(const Entity& other) const { return id != other.id; }
    operator bool() const { return id != 0; }
};