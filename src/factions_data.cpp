#include "../include/factions_data.hpp"

namespace faction_data
{
    enum class FactionID : uint8_t
    {
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
        kKeepersInIron,     // K
        kTotalFactions
    };

    enum class ExpandedScore : uint8_t
    {
        // Maintaining Continuity with factionID is more important than maintaining continuity with CardID, since the position of each could be moved.
        kAlliedWithMarquiseDeCat = static_cast<uint8_t>(FactionID::kMarquiseDeCat),
        kAlliedWithEyrieDynasty = static_cast<uint8_t>(FactionID::kEyrieDynasty),
        kAlliedWithWoodlandAlliance = static_cast<uint8_t>(FactionID::kWoodlandAlliance),
        kAlliedWithLizardCult = static_cast<uint8_t>(FactionID::kLizardCult),
        kAlliedWithRiverfolkCompany = static_cast<uint8_t>(FactionID::kRiverfolkCompany),
        kAlliedWithUndergroundDuchy = static_cast<uint8_t>(FactionID::kUndergroundDuchy),
        kAlliedWithCorvidConspiracy = static_cast<uint8_t>(FactionID::kCorvidConspiracy),
        kAlliedWithLordOfTheHundreds = static_cast<uint8_t>(FactionID::kLordOfTheHundreds),
        kAlliedWithKeepersInIron = static_cast<uint8_t>(FactionID::kKeepersInIron),

        kMouseDominance = static_cast<uint8_t>(FactionID::kTotalFactions) + static_cast<uint8_t>(card_data::CardID::kMouseDominance),
        kFoxDominance = static_cast<uint8_t>(FactionID::kTotalFactions) + static_cast<uint8_t>(card_data::CardID::kFoxDominance),
        kRabbitDominance = static_cast<uint8_t>(FactionID::kTotalFactions) + static_cast<uint8_t>(card_data::CardID::kRabbitDominance),
        kBirdDominance = static_cast<uint8_t>(FactionID::kTotalFactions) + static_cast<uint8_t>(card_data::CardID::kBirdDominance),

        k0 = std::max({kMouseDominance, kFoxDominance, kRabbitDominance, kBirdDominance}) + 1,
        k1,
        k2,
        k3,
        k4,
        k5,
        k6,
        k7,
        k8,
        k9,
        k10,
        k11,
        k12,
        k13,
        k14,
        k15,
        k16,
        k17,
        k18,
        k19,
        k20,
        k21,
        k22,
        k23,
        k24,
        k25,
        k26,
        k27,
        k28,
        k29,
        k30,
    };

    template <IsValidFaction FactionType>
    template<IsUnsignedIntegralOrEnum OutputType, uint16_t... Indices>
    OutputType Faction<FactionType>::unroll_bytes(uint8_t byteIndex, std::index_sequence<Indices...>) const{
        OutputType value = 0;
        ((value |= static_cast<OutputType>(factionData[byteIndex + Indices]) << (Indices * 8)), ...);
        return value;
    }


    template <IsValidFaction FactionType>
    template <IsUnsignedIntegralOrEnum OutputType, uint16_t shift, uint16_t width>
    [[nodiscard]] OutputType Faction<FactionType>::read_bits() const
    {
        static_assert(width > 0, "Width cannot be zero");
        static_assert(width <= sizeof(OutputType) * 8, "Width exceeds output type bit capacity");

        // THIS IS ONLY COMPILE TIME CHECKING PLEASE DON'T BE STUPID, FORGET AND RUIN IT
        constexpr uint8_t lastByte = (shift + width - 1) / 8;
        static_assert(lastByte < sizeof(factionData), "Not enough data to read requested bits");

        constexpr uint8_t byteIndex = shift / 8;
        constexpr uint8_t bitOffset = shift % 8;

        // Fast path: byte-aligned reads can use direct memcpy when width matches the type size exactly
        if constexpr (bitOffset == 0 && width == sizeof(OutputType) * 8)
        {
            OutputType value;
            std::memcpy(&value, &factionData[byteIndex], sizeof(OutputType));
            return value;
        }
        // Type-specific optimized paths

        else if constexpr (
            std::is_same_v<Clean_t<OutputType>, uint8_t> ||
            (std::is_enum_v<Clean_t<OutputType>> &&
            std::is_same_v<std::underlying_type_t<Clean_t<OutputType>>, uint8_t>)
        )
        {
            // Specialized for uint8_t or enum with uint8_t underlying type

            uint8_t value = 0;
            std::memcpy(&value, &factionData[byteIndex], std::min<size_t>(1, sizeof(factionData) - byteIndex));

            if constexpr (bitOffset + width > 8)
            {
                // Handle case where we need bits from a second byte
                value |= factionData[byteIndex + 1] << (8 - bitOffset);
            }

            value >>= bitOffset;

            if constexpr (width < 8) {
                constexpr uint8_t mask = (width < 8) ? ((1U << width) - 1) : 0xFF;
                value &= mask;
            }

            return static_cast<OutputType>(value);
        }
        else if constexpr (
            std::is_same_v<Clean_t<OutputType>, uint16_t> ||
            (std::is_enum_v<Clean_t<OutputType>> &&
            std::is_same_v<std::underlying_type_t<Clean_t<OutputType>>, uint16_t>)
        )
        {
            // Specialized for uint16_t or enum with uint16_t underlying type

            uint16_t value = 0;
            constexpr uint8_t minBytes = std::min<uint8_t>(2, sizeof(factionData) - byteIndex);
            if constexpr (std::endian::native == std::endian::little) {
                std::memcpy(&value, &factionData[byteIndex], minBytes);
            } else {
                value = unroll_bytes<uint16_t>(byteIndex, std::make_index_sequence<minBytes>{});
            }

            if constexpr (bitOffset + width > 16)
            {
                // Handle case where we need bits from a third byte
                value |= factionData[byteIndex + 2] << (16 - bitOffset);
            }

            value >>= bitOffset;

            if constexpr (width < 16) {
                constexpr uint16_t mask = (width < 16) ? ((1U << width) - 1) : 0xFFFF;
                value &= mask;
            }

            return static_cast<OutputType>(value);
        }
        else if constexpr (
            std::is_same_v<Clean_t<OutputType>, uint32_t> ||
            (std::is_enum_v<Clean_t<OutputType>> &&
            std::is_same_v<std::underlying_type_t<Clean_t<OutputType>>, uint32_t>)
        )
        {
            // Specialized for uint32_t or enum with uint32_t underlying type

            uint32_t value = 0;
            constexpr uint8_t minBytes = std::min<uint8_t>(4, sizeof(factionData) - byteIndex);
            if constexpr (std::endian::native == std::endian::little) {
                std::memcpy(&value, &factionData[byteIndex], minBytes);
            } else {
                value = unroll_bytes<uint32_t>(byteIndex, std::make_index_sequence<minBytes>{});
            }

            if constexpr (bitOffset + width > 32)
            {
                // Handle case where we need bits from a fifth byte
                value |= factionData[byteIndex + 4] << (32 - bitOffset);
            }

            value >>= bitOffset;

            if constexpr (width < 32) {
                constexpr uint32_t mask = (width < 32) ? ((1U << width) - 1) : 0xFFFFFFFF;
                value &= mask;
            }
            return static_cast<OutputType>(value);
        }
        else if constexpr (
            std::is_same_v<Clean_t<OutputType>, uint64_t> ||
            (std::is_enum_v<Clean_t<OutputType>> &&
            std::is_same_v<std::underlying_type_t<Clean_t<OutputType>>, uint64_t>)
        )
        {
            // Specialized for uint64_t or enum with uint64_t underlying type

            uint64_t value = 0;
            constexpr uint8_t minBytes = std::min<uint8_t>(8, sizeof(factionData) - byteIndex);
            if constexpr (std::endian::native == std::endian::little) {
                std::memcpy(&value, &factionData[byteIndex], minBytes);
            } else {
                value = unroll_bytes<uint64_t>(byteIndex, std::make_index_sequence<minBytes>{});
            }

            if constexpr (bitOffset + width > 64)
            {
                // Handle case where we need bits from a ninth byte
                value |= factionData[byteIndex + 8] << (64 - bitOffset);
            }

            value >>= bitOffset;

            if constexpr (width < 64) {
                constexpr uint64_t mask = (width < 64) ? ((1U << width) - 1) : 0xFFFFFFFFFFFFFFFF;
                value &= mask;
            }
            return static_cast<OutputType>(value);
        }
        else
        {
            // Generic fallback (including other enum types)
            OutputType value = 0;
            constexpr uint8_t bytes_needed = (bitOffset + width + 7) / 8;

            if constexpr (std::endian::native == std::endian::little)
            {
                std::memcpy(&value, &factionData[byteIndex], std::min(bytes_needed, sizeof(OutputType)));
            }
            else
            {
                value = unroll_bytes<OutputType>(byteIndex, std::make_index_sequence<bytes_needed>{});
            }

            value >>= bitOffset;

            if constexpr (width < sizeof(OutputType) * 8)
            {
                constexpr OutputType mask = (OutputType(1) << width) - 1;
                value &= mask;
            }

            return value;
        }
    }

    template <IsValidFaction FactionType>
    template <IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
    [[nodiscard]] OutputType Faction<FactionType>::read_bits() const
    {
        static_assert(elementWidth > 0, "Element width cannot be zero");
        static_assert(elementWidth <= 8, "Element width cannot exceed one byte");

        // Calculate the index of the last byte we'll need to access
        constexpr uint8_t lastByte = (shift + (sizeof(OutputType) - 1) * elementWidth) / 8;
        static_assert(lastByte < sizeof(factionData), "Not enough data to read requested bits");

        OutputType value = {};

        //maybe do SIMD but this is probably good enough
        // Fast path: contiguous, byte-aligned data can use direct memcpy
        if constexpr (elementWidth == 8 && (shift % 8 == 0))
        {
            if constexpr (std::endian::native == std::endian::little)
            {
                std::memcpy(&value, &factionData[shift / 8], sizeof(OutputType));
            }
            else
            {
                auto unroll = [&](auto... i)
                { (value[i] = static_cast<OutputType::value_type>(factionData[shift + i]), ...); };

                unroll(std::make_index_sequence<sizeof(OutputType)>{});
            }

            return value;
        }
        else
        {
            // General path for non-aligned or partial byte reads (unrolled at compile time)
            
            // Create bit mask to extract only 'elementWidth' bits
            constexpr uint8_t mask = (1U << elementWidth) - 1;

            auto unroll = [&](auto... i)
            {
                (
                    // For each element in the output array:
                    value[i] = static_cast<OutputType::value_type>(

                        // Calculate the starting bit position for this element
                        // and the corresponding byte index
                        // (startByte = (shift + i * elementWidth) / 8)
                        [&]
                        {
                            constexpr uint16_t bitPos = shift + i * elementWidth;
                            constexpr uint8_t startByte = bitPos / 8;
                            constexpr uint8_t bitOffset = bitPos % 8;

                            // Always read the current byte
                            uint16_t value = factionData[startByte];

                            // If the bits for this element cross a byte boundary,
                            // also read the next byte and combine
                            if constexpr (bitOffset + elementWidth > 8)
                            {
                                value |= static_cast<uint16_t>(factionData[startByte + 1]) << 8;
                            }

                            // Shift right to align the desired bits to LSB
                            value >>= bitOffset;

                            // Mask to keep only the bits we're interested in
                            return value & mask;
                        }()),
                    // Fold expression repeats this for all indices
                    ...);
            };

            // Execute the unrolled loop using compile-time index sequence
            unroll(std::make_index_sequence<sizeof(OutputType)>{});
        }
        return value;
    };

    template<IsValidFaction FactionType>
    template<IsUnsignedIntegralOrEnum InputType>
    void Faction<FactionType>::write_bits(const InputType &value, uint16_t shift, uint16_t width) {
        [[unlikely]] if (width == 0)
            throw std::invalid_argument("Width cannot be zero");
        [[unlikely]] if (width > sizeof(InputType) * 8)
            throw std::invalid_argument("Width exceeds input type bit capacity");

        const uint8_t lastByte = (shift + width - 1) / 8;
        [[unlikely]] if (lastByte >= sizeof(factionData))
            throw std::out_of_range("Not enough data to write requested bits");

        const uint8_t byteIndex = shift / 8;
        const uint8_t bitOffset = shift % 8;

        const InputType value_mask = (width < sizeof(InputType) * 8) ? ((InputType(1) << width) - 1) : InputType(~0);
        const InputType masked_value = value & value_mask;

        if constexpr (std::is_same_v<InputType, uint8_t>) {
            // Specialized for uint8_t
            const uint8_t mask = (1U << width) - 1;
            const uint8_t clear_mask = ~(mask << bitOffset);
            factionData[byteIndex] = (factionData[byteIndex] & clear_mask) | (static_cast<uint8_t>(masked_value) << bitOffset);
            return;
        } else if constexpr (std::is_same_v<InputType, uint16_t>) {
            // Specialized for uint16_t
            uint16_t twoBytes = 0;
            std::memcpy(&twoBytes, &factionData[byteIndex], std::min<size_t>(2, sizeof(factionData) - byteIndex));
            const uint16_t mask = (1U << width) - 1;
            const uint16_t clear_mask = ~(mask << bitOffset);
            twoBytes = (twoBytes & clear_mask) | (static_cast<uint16_t>(masked_value) << bitOffset);
            std::memcpy(&factionData[byteIndex], &twoBytes, std::min<size_t>(2, sizeof(factionData) - byteIndex));
            return;
        } else if constexpr (std::is_same_v<InputType, uint32_t>) {
            // Specialized for uint32_t
            uint32_t four_bytes = 0;
            std::memcpy(&four_bytes, &factionData[byteIndex], std::min<size_t>(4, sizeof(factionData) - byteIndex));
            const uint32_t mask = (1U << width) - 1;
            const uint32_t clear_mask = ~(mask << bitOffset);
            four_bytes = (four_bytes & clear_mask) | (static_cast<uint32_t>(masked_value) << bitOffset);
            std::memcpy(&factionData[byteIndex], &four_bytes, std::min<size_t>(4, sizeof(factionData) - byteIndex));
            return;
        } else if constexpr (std::is_same_v<InputType, uint64_t>) {
            // Specialized for uint64_t
            uint64_t eight_bytes = 0;
            std::memcpy(&eight_bytes, &factionData[byteIndex], std::min<size_t>(8, sizeof(factionData) - byteIndex));
            const uint64_t mask = (uint64_t(1) << width) - 1;
            const uint64_t clear_mask = ~(mask << bitOffset);
            eight_bytes = (eight_bytes & clear_mask) | (static_cast<uint64_t>(masked_value) << bitOffset);
            std::memcpy(&factionData[byteIndex], &eight_bytes, std::min<size_t>(8, sizeof(factionData) - byteIndex));
            return;
        } else {
            // Generic fallback (including enums)
            const size_t bytes_needed = (bitOffset + width + 7) / 8;
            InputType existing_data = 0;
            const size_t bytes_to_read = std::min(bytes_needed, sizeof(InputType));
            if constexpr (std::endian::native == std::endian::little) {
                std::memcpy(&existing_data, &factionData[byteIndex], bytes_to_read);
            } else {
                for (size_t i = 0; i < bytes_to_read; ++i) {
                    existing_data |= static_cast<InputType>(factionData[byteIndex + i]) << (i * 8);
                }
            }
            const InputType clear_mask = (width < sizeof(InputType) * 8) ? ~(((InputType(1) << width) - 1) << bitOffset) : InputType(0);
            const InputType combined = (existing_data & clear_mask) | (masked_value << bitOffset);
            const size_t bytesToWrite = std::min(bytes_needed, sizeof(InputType));
            if constexpr (std::endian::native == std::endian::little) {
                std::memcpy(&factionData[byteIndex], &combined, bytesToWrite);
            } else {
                for (size_t i = 0; i < bytesToWrite; ++i) {
                    factionData[byteIndex + i] = static_cast<uint8_t>(combined >> (i * 8));
                }
            }
        }
    }
}