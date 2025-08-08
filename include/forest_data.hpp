#pragma once

#include "token_data.hpp"
#include "board_data.hpp"
#include "game_data.hpp"

#include <cstdint>
#include <array>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <expected>
#include <bitset>

namespace game_data
{
namespace board_data
{
namespace forest_data
{

namespace faction_data = ::game_data::faction_data;
namespace token_data = ::game_data::token_data;
namespace board_data = ::game_data::board_data;
namespace game_data = ::game_data;


struct RelicError {
    enum class Code : uint8_t {
        kInvalidRelicType,
        kCountExceedsMaximum,
        kDuplicateRelicTypes,
        kUnknownError
    } code;

    static constexpr std::array<std::string_view, 4> kMessages = {
        "Invalid relic type provided",
        "Relic count exceeds maximum for that relic type",
        "Duplicate relic types are not allowed",
        "Unknown error"
    };

    static std::string_view to_string(Code code) {
        uint8_t idx = static_cast<uint8_t>(code);
        if (idx < kMessages.size()) return kMessages[idx];
        return kMessages.back();
    }

    std::string_view message() const { return to_string(code); }
};

class Forest
{
    /*
    12 Bits: Relics {
        Bit 1: Figure Value 1 (0-1)
        Bit 2: Figure Value 2 (0-1)
        Bits 3-4: Figure Value 3 (0-2, 3 = unused)
        Bit 5: Tablet Value 1 (0-1)
        Bit 6: Tablet Value 2 (0-1)
        Bits 7-8: Tablet Value 3 (0-2, 3 = unused)
        Bit 9: Jewelry Value 1 (0-1)
        Bit 10: Jewelry Value 2 (0-1)
        Bits 11-12: Jewelry Value 3 (0-2, 3 = unused)
    }

    2 Bits: Vagabonds {
        Bit 1: Vagabond 1 (0-1)
        Bit 2: Vagabond 2 (0-1)
    }

    12 Bits: Connected Clearings {
    1 Bit: Is connected? (0-1)
    } x 12 for each clearing
    */

public:
    // Forest(const std::vector<uint8_t> &clearingConnections) 
    // {
    //     set_clearing_connections(clearingConnections);
    // }

    template<token_data::Token relic>
    [[nodiscard]] inline std::expected<uint8_t, RelicError> get_relic_count() const;
    template<token_data::Token relic, uint8_t newCount>
    inline void set_relic_count();
    template<token_data::Token desiredRelic>
    [[nodiscard]] inline std::expected<void, RelicError> set_relic_count(uint8_t newCount);

    template <token_data::RelicType relicType>
    [[nodiscard]] uint8_t get_relic_type_count() const;

    template <uint8_t whichVagabond>
    [[nodiscard]] bool is_vagabond_present() const;
    template <uint8_t whichVagabond>
    void set_is_vagabond_present(bool value);

private:

    struct RelicDataInfo
    {
        uint8_t offset;
        uint8_t width;
        uint8_t maxCount;
    };

    static constexpr std::array<RelicDataInfo, 9> kRelicDataInfoField = {{
        {0, 1, 1}, // Figure Value 1
        {1, 1, 1}, // Figure Value 2
        {2, 2, 2}, // Figure Value 3
        {4, 1, 1}, // Tablet Value 1
        {5, 1, 1}, // Table Value 2
        {6, 2, 2}, // Tablet Value 3
        {8, 1, 1}, // Jewelry Value 1
        {9, 1, 1}, // Jewelry Value 2
        {10, 2, 2} // Jewelry Value 3
    }};

    static constexpr uint8_t kRelicsBits = 12;

    static constexpr uint8_t kVagabondBits = 1;
    static constexpr uint8_t kTotalVagabonds = 2;
    static constexpr uint8_t kVagabondsBits = kVagabondBits * kTotalVagabonds;

    static constexpr uint8_t kConnectedClearingsCountBits = 3;
    static constexpr uint8_t kClearingConnectionBits = 4;
    static constexpr uint8_t kMaxClearingConnections = 6;
    static constexpr uint8_t kConnectedClearingsBits = kClearingConnectionBits * kMaxClearingConnections;

    static constexpr uint8_t kRelicsOffset = 0;
    static constexpr uint8_t kVagabondsOffset = kRelicsOffset + kRelicsBits;
    static constexpr uint8_t kConnectedClearingsCountOffset = kVagabondsOffset + kVagabondsBits;
    static constexpr uint8_t kConnectedClearingOffset = kConnectedClearingsCountOffset + kConnectedClearingsCountBits;

    std::array<uint8_t, (kConnectedClearingOffset + kConnectedClearingsBits + 7) / 8> forestData;

    // Wrappers for read and write bits functions to allow for ease of use
    template <game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t shift, uint16_t width>
    [[nodiscard]] inline OutputType read_bits() const {
        return game_data::read_bits<OutputType, sizeof(forestData), shift, width>(forestData);
    }

    template <game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t width>
    [[nodiscard]] inline OutputType read_bits(uint16_t shift) const {
        return game_data::read_bits<OutputType, sizeof(forestData), width>(forestData, shift);
    }

    template <game_data::IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
    [[nodiscard]] inline OutputType read_bits() const {
        return game_data::read_bits<OutputType, sizeof(forestData), shift, elementWidth>(forestData);
    }

    template <game_data::IsValidByteArray OutputType, uint8_t elementWidth>
    [[nodiscard]] inline OutputType read_bits(uint16_t shift) const {
        return game_data::read_bits<OutputType, sizeof(forestData), elementWidth>(forestData, shift);
    }

    template <game_data::IsUnsignedIntegralOrEnum InputType, uint16_t shift, uint16_t width>
    inline void write_bits(const InputType &value) {
        game_data::write_bits<InputType, sizeof(forestData), shift, width>(forestData, value);
    }

    template <game_data::IsUnsignedIntegralOrEnum InputType, uint16_t width>
    inline void write_bits(const InputType &value, uint16_t shift) {
        game_data::write_bits<InputType, sizeof(forestData), width>(forestData, value, shift);
    }

    template <game_data::IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
    inline void write_bits(const InputType &value) {
        game_data::write_bits<InputType, sizeof(forestData), shift, elementWidth>(forestData, value);
    }

    template <game_data::IsValidByteArray InputType, uint8_t elementWidth>
    inline void write_bits(const InputType &value, uint16_t shift) {
        game_data::write_bits<InputType, sizeof(forestData), elementWidth>(forestData, value, shift);
    }

    [[nodiscard]] inline std::expected<uint8_t, ConnectionError> get_clearing_connection_count() {
        return board_data::get_basic_connection_count<kConnectedClearingsCountBits, kConnectedClearingsCountOffset, kMaxClearingConnections, sizeof(forestData)>(forestData);
    }

    template <uint8_t newCount>
    inline void set_clearing_connection_count() {
        return board_data::set_basic_connection_count<kConnectedClearingsCountBits, kConnectedClearingsCountOffset, kMaxClearingConnections, sizeof(forestData), newCount>(forestData);
    }

    [[nodiscard]] inline std::expected<void, ConnectionError> set_clearing_connection_count(uint8_t newCount) {
        return board_data::set_basic_connection_count<kConnectedClearingsCountBits, kConnectedClearingsCountOffset, kMaxClearingConnections, sizeof(forestData)>(forestData, newCount);
    }

    [[nodiscard]] inline std::expected<std::vector<uint8_t>, ConnectionError> get_clearing_connections() {
        return board_data::get_basic_connections<kConnectedClearingsCountBits, kConnectedClearingsCountOffset, kMaxClearingConnections, kClearingConnectionBits, kConnectedClearingOffset, sizeof(forestData)>(forestData);
    }

    [[nodiscard]] inline std::expected<void, ConnectionError> set_clearing_connections(const std::vector<uint8_t> &newConnections) {
        return board_data::set_basic_connections<kConnectedClearingsCountBits, kConnectedClearingsCountOffset, kMaxClearingConnections, kClearingConnectionBits, kConnectedClearingOffset, sizeof(forestData)>(forestData, newConnections);
    }

    [[nodiscard]] inline std::expected<void, ConnectionError> add_clearing_connections(const std::vector<uint8_t> &newConnections) {
        return board_data::add_basic_connections<kConnectedClearingsCountBits, kConnectedClearingsCountOffset, kMaxClearingConnections, kClearingConnectionBits, kConnectedClearingOffset, sizeof(forestData)>(forestData, newConnections);
    }

    [[nodiscard]] std::expected<void, ConnectionError> remove_clearing_connections(const std::vector<uint8_t> &indices) {
        return board_data::remove_basic_connections<kConnectedClearingsCountBits, kConnectedClearingsCountOffset, kMaxClearingConnections, kClearingConnectionBits, kConnectedClearingOffset, sizeof(forestData)>(forestData, indices);
    }
};
} // forest_data
} // board_data
} // game_data