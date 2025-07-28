#pragma once

#include <cstdint>

struct Entity
{
    using IdType = std::uint32_t;
    IdType id{0};

    // Operadores �tiles para comparaci�n y conversi�n
    bool operator==(const Entity& other) const { return id == other.id; }
    bool operator!=(const Entity& other) const { return id != other.id; }
    operator bool() const { return id != 0; }
};