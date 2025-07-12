#pragma once

#include <concepts>
#include <array>
#include <cstdint>

namespace game_data
{
    template<typename T>
    concept IsUnsignedIntegralOrEnum = std::unsigned_integral<T> || 
        (std::is_enum_v<T> && std::unsigned_integral<std::underlying_type_t<T>>);

    template<typename T>
    using Clean_t = std::remove_cv_t<std::remove_reference_t<T>>;

    // Note: I had problems with template metaprograming when dealing with const so I spammed Clean_t everywhere. Some uses might be unnecessary but idc tbh its good enough
    template <typename T>
    concept IsValidByteArray =
        // Check that T is an std::array or std::vector of some type and size
        (std::is_same_v<T, std::array<Clean_t<typename T::value_type>, std::tuple_size_v<T>>> && std::tuple_size_v<T> > 0) &&
        (
            // The element type is uint8_t
            std::is_same_v<Clean_t<typename T::value_type>, uint8_t> ||
            // Or, the element type is an enum whose underlying type is uint8_t
            (std::is_enum_v<Clean_t<typename T::value_type>> && std::is_same_v<std::underlying_type_t<Clean_t<typename T::value_type>>, uint8_t>));

    template<typename T>
    concept IsUint8OrEnumUint8 = std::same_as<T, uint8_t> ||
        (std::is_enum_v<T> && std::same_as<std::underlying_type_t<T>, uint8_t>);
} // game_data