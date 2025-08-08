#pragma once

#include <concepts>
#include <array>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <bit>

namespace game_data
{
template <typename T>
using Clean_t = std::remove_cv_t<std::remove_reference_t<T>>;

template<typename T, bool = std::is_enum_v<T>>
struct UnderlyingTypeOrVoidHelper {
    using type = std::conditional_t<std::is_same_v<T, bool>, uint8_t, void>;
};

template<typename T>
struct UnderlyingTypeOrVoidHelper<T, true> {
    using type = std::underlying_type_t<T>;
};

template<typename T>
using UnderlyingTypeOrVoid_t = typename UnderlyingTypeOrVoidHelper<T>::type;

template <typename T>
concept IsUnsignedIntegralOrEnum = std::unsigned_integral<T> || 
    std::is_same_v<UnderlyingTypeOrVoid_t<T>, uint8_t>;


// Note: I had problems with template metaprograming when dealing with const so I spammed Clean_t everywhere. Some uses might be unnecessary but idc tbh its good enough
template <typename T>
concept IsValidByteArray =
    // Check that T is an std::array or std::vector of some type and size
    (std::is_same_v<T, std::array<Clean_t<typename T::value_type>, std::tuple_size<T>::value>> && std::tuple_size<T>::value > 0) &&
    (
        // The element type is uint8_t
        std::is_same_v<Clean_t<typename T::value_type>, uint8_t> ||
        // Or, the element type is an enum whose underlying type is uint8_t
        std::is_same_v<UnderlyingTypeOrVoid_t<typename T::value_type>, uint8_t>);

template <typename T>
concept IsUint8OrEnumUint8 = std::same_as<T, uint8_t> ||
    std::is_same_v<UnderlyingTypeOrVoid_t<T>, uint8_t>;

// only here to get around circular dependencies
namespace faction_data {
enum class FactionID : uint8_t {
    kMarquiseDeCat,     // A
    kEyrieDynasty,      // B
    kWoodlandAlliance,  // C
    kVagabond1,         // D
    kVagabond2,         // E
    kLizardCult,        // F
    kRiverfolkCompany,  // G
    kUndergroundDuchy,  // H
    kCorvidConspiracy,  // I
    kLordOfTheHundreds, // J
    kKeepersInIron      // K
};
} // namespace faction_data

// Unfortunately it seems these need to be in the header otherwise the compiler won't generate template the needed template instantiations 
template <IsUnsignedIntegralOrEnum OutputType, size_t N, uint16_t shift, uint16_t width>
[[nodiscard]] OutputType read_bits(const std::array<uint8_t, N> &data)
{
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= sizeof(OutputType) * 8, "Width exceeds output type bit capacity");
    constexpr uint8_t lastByte = (shift + width - 1) / 8;
    static_assert(lastByte < sizeof(data), "Not enough data to read requested bits");
    constexpr uint8_t byteIndex = shift / 8;
    constexpr uint8_t bitOffset = shift % 8;
    OutputType value = OutputType(0);
    if constexpr (bitOffset == 0 && width == sizeof(OutputType) * 8)
    {
        std::memcpy(&value, &data[byteIndex], sizeof(OutputType));
    }
    else if constexpr (
        std::is_same_v<Clean_t<OutputType>, uint8_t> ||
        std::is_same_v<UnderlyingTypeOrVoid_t<OutputType>, uint8_t>
    )
    {
        uint8_t temp = data[byteIndex];
        if constexpr (bitOffset + width > 8)
            temp |= data[byteIndex + 1] << (8 - bitOffset);
        temp >>= bitOffset;
        if constexpr (width < 8) {
            constexpr uint8_t mask = (1U << width) - 1;
            temp &= mask;
        }
        value = static_cast<OutputType>(temp);
    }
    else
    {
        OutputType temp = 0;
        constexpr uint8_t bytesNeeded = (width + 7) / 8;
        std::memcpy(&temp, &data[byteIndex], bytesNeeded);
        if constexpr (std::endian::native == std::endian::big)
            temp = std::byteswap(temp);
        temp >>= bitOffset;
        if constexpr (width < sizeof(OutputType) * 8)
        {
            constexpr OutputType mask = (OutputType(1) << width) - 1;
            temp &= mask;
        }
        value = static_cast<OutputType>(temp);
    }
    return value;
}

template <IsUnsignedIntegralOrEnum OutputType, size_t N, uint16_t width>
[[nodiscard]] OutputType read_bits(const std::array<uint8_t, N> &data, uint16_t shift)
{
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= sizeof(OutputType) * 8, "Width exceeds output type bit capacity");
    // const uint8_t lastByte = (shift + width - 1) / 8;
    // [[unlikely]] if (lastByte >= sizeof(data))
    //     throw std::invalid_argument("Not enough data to read requested bits");
    const uint8_t byteIndex = shift / 8;
    const uint8_t bitOffset = shift % 8;
    OutputType value = OutputType(0);
    if (bitOffset == 0 && width == sizeof(OutputType) * 8)
    {
        std::memcpy(&value, &data[byteIndex], sizeof(OutputType));
    }
    else if constexpr (
        std::is_same_v<Clean_t<OutputType>, uint8_t> ||
        std::is_same_v<UnderlyingTypeOrVoid_t<OutputType>, uint8_t>
    )
    {
        uint8_t temp = data[byteIndex];
        if (bitOffset + width > 8)
            temp |= data[byteIndex + 1] << (8 - bitOffset);
        temp >>= bitOffset;
        if constexpr (width < 8) {
            constexpr uint8_t mask = (1U << width) - 1;
            temp &= mask;
        }
        value = static_cast<OutputType>(temp);
    }
    else
    {
        OutputType temp = 0;
        constexpr uint8_t bytesNeeded = (width + 7) / 8;
        std::memcpy(&temp, &data[byteIndex], bytesNeeded);
        if constexpr (std::endian::native == std::endian::big)
            temp = std::byteswap(temp);
        temp >>= bitOffset;
        if constexpr (width < sizeof(OutputType) * 8)
        {
            constexpr OutputType mask = (OutputType(1) << width) - 1;
            temp &= mask;
        }
        value = static_cast<OutputType>(temp);
    }
    return value;
}

template <IsValidByteArray OutputType, size_t N, uint16_t shift, uint8_t elementWidth>
[[nodiscard]] OutputType read_bits(const std::array<uint8_t, N> &data)
{
    static_assert(elementWidth > 0, "Element width cannot be zero");
    static_assert(elementWidth <= 8, "Element width cannot exceed one byte");
    constexpr uint8_t lastByte = (shift + (sizeof(OutputType) - 1) * elementWidth) / 8;
    static_assert(lastByte < sizeof(data), "Not enough data to read requested bits");
    OutputType value = {};
    if constexpr (elementWidth == 8 && (shift % 8 == 0))
    {
        std::memcpy(&value, &data[shift / 8], sizeof(OutputType));
    }
    else
    {
        auto unroll = [&]<size_t... i>(std::index_sequence<i...>)
        {
            (([&] {
                    constexpr uint16_t bitPos = shift + i * elementWidth;
                    constexpr uint8_t byteIndex = bitPos / 8;
                    constexpr uint8_t bitOffset = bitPos % 8;
                    uint8_t byteValue = data[byteIndex];
                    if constexpr (bitOffset + elementWidth > 8)
                    {
                        byteValue |= data[byteIndex + 1] << (8 - bitOffset);
                    }
                    byteValue >>= bitOffset;
                    if constexpr (elementWidth < 8) {
                        constexpr uint8_t mask = (1U << elementWidth) - 1;
                        byteValue &= mask;
                    }
                    value[i] = static_cast<OutputType::value_type>(byteValue); 
                }()),
            ...);
        };
        unroll(std::make_index_sequence<sizeof(OutputType)>{});
    }
    return value;
}

template <IsValidByteArray OutputType, size_t N, uint8_t elementWidth>
[[nodiscard]] OutputType read_bits(const std::array<uint8_t, N> &data, uint16_t shift) {
    static_assert(elementWidth > 0, "Element width cannot be zero");
    static_assert(elementWidth <= 8, "Element width cannot exceed one byte");
    // const uint8_t lastByte = (shift + (sizeof(OutputType) - 1) * elementWidth) / 8;
    // [[unlikely]] if (lastByte >= sizeof(data))
    //     throw std::invalid_argument("Not enough data to read requested bits");
    OutputType value = {};
    if (elementWidth == 8 && (shift % 8 == 0))
    {
        std::memcpy(&value, &data[shift / 8], sizeof(OutputType));
    }
    else
    {
        auto unroll = [&]<size_t... i>(std::index_sequence<i...>)
        {
            (([&] {
                    const uint16_t bitPos = shift + i * elementWidth;
                    const uint8_t byteIndex = bitPos / 8;
                    const uint8_t bitOffset = bitPos % 8;
                    uint8_t byteValue = data[byteIndex];
                    if (bitOffset + elementWidth > 8)
                    {
                        byteValue |= data[byteIndex + 1] << (8 - bitOffset);
                    }
                    byteValue >>= bitOffset;
                    if constexpr (elementWidth < 8) {
                        constexpr uint8_t mask = (1U << elementWidth) - 1;
                        byteValue &= mask;
                    }
                    value[i] = static_cast<OutputType::value_type>(byteValue); 
                }()),
            ...);
        };
        unroll(std::make_index_sequence<sizeof(OutputType)>{});
    }
    return value;
}

template <IsUnsignedIntegralOrEnum InputType, size_t N, uint16_t shift, uint16_t width>
void write_bits(std::array<uint8_t, N> &data, const InputType &value) {
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= sizeof(InputType) * 8, "Width exceeds output type bit capacity");
    constexpr uint8_t lastByte = (shift + width - 1) / 8;
    static_assert(lastByte < sizeof(data), "Not enough data to write requested bits");
    constexpr uint8_t byteIndex = shift / 8;
    constexpr uint8_t bitOffset = shift % 8;
    if constexpr (bitOffset == 0 && width == sizeof(InputType) * 8)
    {
        std::memcpy(&data[byteIndex], &value, sizeof(InputType));
    }
    else if constexpr (
        std::is_same_v<Clean_t<InputType>, uint8_t> ||
        std::is_same_v<UnderlyingTypeOrVoid_t<InputType>, uint8_t>
    )
    {
        uint8_t value_uint8 = static_cast<uint8_t>(value);
        constexpr uint8_t mask = (width < 8) ? ((1U << width) - 1) : 0xFF;
        value_uint8 &= mask;
        data[byteIndex] &= ~(mask << bitOffset);
        data[byteIndex] |= (value_uint8 << bitOffset);
        if constexpr (bitOffset + width > 8) {
            constexpr uint8_t nextMask = (mask >> (8 - bitOffset));
            data[byteIndex + 1] &= ~nextMask;
            data[byteIndex + 1] |= (value_uint8 >> (8 - bitOffset)) & nextMask;
        }
    }
    else
    {
        InputType mutableValue = value;
        if constexpr (std::endian::native == std::endian::big)
            mutableValue = std::byteswap(mutableValue);
        constexpr uint8_t bytesNeeded = (width + 7) / 8;
        InputType orig = 0;
        constexpr InputType mask = (width < bytesNeeded * 8) ? (~InputType(0)) >> (sizeof(InputType) * 8 - width) : ~InputType(0);
        mutableValue &= mask;
        std::memcpy(&orig, &data[byteIndex], bytesNeeded);
        orig &= ~(mask << bitOffset);
        orig |= (mutableValue << bitOffset);
        std::memcpy(&data[byteIndex], &orig, bytesNeeded);
        if constexpr (bitOffset + width > bytesNeeded * 8) {
            constexpr uint8_t nextMask = (mask >> (bytesNeeded * 8 - bitOffset));
            data[byteIndex + bytesNeeded] &= ~nextMask;
            data[byteIndex + bytesNeeded] |= (mutableValue >> (bytesNeeded * 8 - bitOffset)) & nextMask;
        }
    }
}

template <IsUnsignedIntegralOrEnum InputType, size_t N, uint16_t width>
void write_bits(std::array<uint8_t, N> &data, const InputType &value, uint16_t shift)
{
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= sizeof(InputType) * 8, "Width exceeds output type bit capacity");
    // const uint8_t lastByte = (shift + width - 1) / 8;
    // [[unlikely]] if (lastByte >= sizeof(data))
    //     throw std::invalid_argument("Not enough data to write requested bits");
    const uint8_t byteIndex = shift / 8;
    const uint8_t bitOffset = shift % 8;
    if (bitOffset == 0 && width == sizeof(InputType) * 8)
    {
        std::memcpy(&data[byteIndex], &value, sizeof(InputType));
    }
    else if constexpr (
        std::is_same_v<Clean_t<InputType>, uint8_t> ||
        std::is_same_v<UnderlyingTypeOrVoid_t<InputType>, uint8_t>
    )
    {
        uint8_t value_uint8 = static_cast<uint8_t>(value);
        constexpr uint8_t mask = (width < 8) ? ((1U << width) - 1) : 0xFF;
        value_uint8 &= mask;
        data[byteIndex] &= ~(mask << bitOffset);
        data[byteIndex] |= (value_uint8 << bitOffset);
        if (bitOffset + width > 8) {
            const uint8_t nextMask = (mask >> (8 - bitOffset));
            data[byteIndex + 1] &= ~nextMask;
            data[byteIndex + 1] |= (value_uint8 >> (8 - bitOffset)) & nextMask;
        }
    }
    else
    {
        InputType mutableValue = value;
        if constexpr (std::endian::native == std::endian::big)
            mutableValue = std::byteswap(mutableValue);
        constexpr uint8_t bytesNeeded = (width + 7) / 8;
        InputType orig = 0;
        constexpr InputType mask = (width < bytesNeeded * 8) ? 
        ((~InputType(0)) >> (sizeof(InputType) * 8 - width)) : ~InputType(0);
        mutableValue &= mask;
        std::memcpy(&orig, &data[byteIndex], bytesNeeded);
        orig &= ~(mask << bitOffset);
        orig |= (mutableValue << bitOffset);
        std::memcpy(&data[byteIndex], &orig, bytesNeeded);
        if (bitOffset + width > bytesNeeded * 8) {
            const uint8_t nextMask = (mask >> (bytesNeeded * 8 - bitOffset));
            data[byteIndex + bytesNeeded] &= ~nextMask;
            data[byteIndex + bytesNeeded] |= (mutableValue >> (bytesNeeded * 8 - bitOffset)) & nextMask;
        }
    }
}

template <IsValidByteArray InputType, size_t N, uint16_t shift, uint8_t elementWidth>
void write_bits(std::array<uint8_t, N> &data, const InputType &value) {
    static_assert(elementWidth > 0, "Element width cannot be zero");
    static_assert(elementWidth <= 8, "Element width cannot exceed one byte");
    constexpr uint8_t lastByte = (shift + (sizeof(InputType) - 1) * elementWidth) / 8;
    static_assert(lastByte < sizeof(data), "Not enough data to write requested bits");
    if constexpr (elementWidth == 8 && (shift % 8 == 0))
    {
        std::memcpy(&data[shift / 8], &value, sizeof(InputType));
    }
    else
    {
        auto unroll = [&]<size_t... i>(std::index_sequence<i...>)
        {
            (([&] {
                constexpr uint16_t bitPos = shift + i * elementWidth;
                constexpr uint8_t byteIndex = bitPos / 8;
                constexpr uint8_t bitOffset = bitPos % 8;
                uint8_t byteValue = static_cast<uint8_t>(value[i]);
                constexpr uint8_t mask = (elementWidth < 8) ? ((1U << elementWidth) - 1) : 0xFF;
                byteValue &= mask;
                data[byteIndex] &= ~(mask << bitOffset);
                data[byteIndex] |= (byteValue << bitOffset);
                if constexpr (bitOffset + elementWidth > 8) {
                    constexpr uint8_t nextMask = (mask >> (8 - bitOffset));
                    data[byteIndex + 1] &= ~nextMask;
                    data[byteIndex + 1] |= (byteValue >> (8 - bitOffset)) & nextMask;
                }                      
            }()),
            ...);
        };
        unroll(std::make_index_sequence<sizeof(InputType)>{});
    }
}

template <IsValidByteArray InputType, size_t N, uint8_t elementWidth>
void write_bits(std::array<uint8_t, N> &data, const InputType& value, uint16_t shift)
{
    static_assert(elementWidth > 0, "Element width cannot be zero");
    static_assert(elementWidth <= 8, "Element width cannot exceed one byte");
    // const uint8_t lastByte = (shift + (sizeof(InputType) - 1) * elementWidth) / 8;
    // [[unlikely]] if (lastByte >= sizeof(data))
    //     throw std::invalid_argument("Not enough data to write requested bits");
    if (elementWidth == 8 && (shift % 8 == 0))
    {
        std::memcpy(&data[shift / 8], &value, sizeof(InputType));
    }
    else
    {
        auto unroll = [&]<size_t... i>(std::index_sequence<i...>)
        {
            (([&] {
                const uint16_t bitPos = shift + i * elementWidth;
                const uint8_t byteIndex = bitPos / 8;
                const uint8_t bitOffset = bitPos % 8;
                uint8_t byteValue = static_cast<uint8_t>(value[i]);
                constexpr uint8_t mask = (elementWidth < 8) ? ((1U << elementWidth) - 1) : 0xFF;
                byteValue &= mask;
                data[byteIndex] &= ~(mask << bitOffset);
                data[byteIndex] |= (byteValue << bitOffset);
                if (bitOffset + elementWidth > 8) {
                    const uint8_t nextMask = (mask >> (8 - bitOffset));
                    data[byteIndex + 1] &= ~nextMask;
                    data[byteIndex + 1] |= (byteValue >> (8 - bitOffset)) & nextMask;
                }                      
            }()),
            ...);
        };
        unroll(std::make_index_sequence<sizeof(InputType)>{});
    }
}

enum class SetupType : uint8_t {
    Normal,
    Advanced
};
} // game_data