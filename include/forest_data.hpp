#pragma once

#include "token_data.hpp"
#include "board_data.hpp"

#include <cstdint>
#include <array>

namespace game_data
{
namespace board_data
{
namespace forest_data
{

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
    Forest() : data({}) {}

    template <typename T = uint8_t>
    [[nodiscard]] T read_bits(uint8_t shift, uint8_t width) const;

    void write_bits(uint32_t value, uint8_t shift, uint8_t width);

    [[nodiscard]] std::array<token_data::Token,
                             (static_cast<uint8_t>(token_data::Token::kJewelryValue3) -
                              static_cast<uint8_t>(token_data::Token::kFigureValue1) + 1)>
    get_relics() const;
    bool set_relic(token_data::Token token, uint8_t count);

    [[nodiscard]] bool get_vagabond(uint8_t index) const;
    bool set_vagabond(uint8_t index, bool value);

    [[nodiscard]] bool get_connected_clearing(uint8_t index) const;
    bool set_connected_clearing(uint8_t index, bool value);

private:

    static constexpr uint8_t kRelicBits = 12;

    static constexpr uint8_t kVagabondBits = 1;
    static constexpr uint8_t kTotalVagabonds = 2;
    static constexpr uint8_t kVagabondsBits = kVagabondBits * kTotalVagabonds;

    static constexpr uint8_t kClearingConnectionBits = 1;
    static constexpr uint8_t kConnectedClearingsBits =
        kClearingConnectionBits * ::game_data::board_data::kTotalClearings;

    static constexpr uint8_t kRelicsShift = 0;
    static constexpr uint8_t kVagabondsShift = kRelicsShift + kRelicBits;
    static constexpr uint8_t kConnectedClearingsShift =
        kVagabondsShift + kVagabondsBits;

    std::array<uint8_t, (kConnectedClearingsShift + kConnectedClearingsBits + 7) / 8> data;
};
} // forest_data
} // board_data
} // game_data