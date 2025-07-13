#pragma once

#include "game_data.hpp"
#include "card_data.hpp"

#include <array>
#include <cstdint>
#include <vector>
#include <concepts>
#include <stdexcept>
#include <algorithm>
#include <cstring>

namespace game_data
{
namespace discard_pile_data
{
class DiscardPile {
    public:
        template<::game_data::IsUint8OrEnumUint8 OutputType, uint16_t shift, uint8_t width>
        [[nodiscard]] OutputType read_bits() const;
        template<::game_data::IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
        [[nodiscard]] OutputType read_bits() const;

        template<::game_data::IsUint8OrEnumUint8 InputType, uint16_t shift, uint16_t width>
        void write_bits(const InputType &value);
        template<::game_data::IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
        void write_bits(const InputType &value);

        void write_bits_runtime(const ::game_data::card_data::CardID &value, uint16_t shift, uint8_t width);
        void write_bits_runtime(const std::vector<::game_data::card_data::CardID> &values, uint16_t shift, uint8_t elementWidth);
    
        [[nodiscard]] inline uint8_t get_discard_pile_size() const;
        template <uint8_t newSize>
        inline void set_discard_pile_size();
        inline void set_discard_pile_size(uint8_t newSize);

        [[nodiscard]] std::vector<::game_data::card_data::CardID> get_discard_pile_contents() const;
        void set_discard_pile_contents(const std::vector<::game_data::card_data::CardID> &newDiscardPile);
        [[nodiscard]] std::vector<::game_data::card_data::CardID> get_cards_in_discard_pile(const std::vector<uint8_t> &desiredCardIndices) const;
        [[nodiscard]] std::vector<::game_data::card_data::CardID> get_cards_in_discard_pile(uint8_t startIndex, uint8_t endIndex) const;
        void set_cards_in_discard_pile(const std::vector<std::pair<uint8_t, ::game_data::card_data::CardID>> &newIndexCardPairs);
        void add_cards_to_discard_pile(const std::vector<::game_data::card_data::CardID> &newCards);
        void remove_cards_from_discard_pile(const std::vector<uint8_t> &indices);
        inline void pop_cards_from_discard_pile(uint8_t popCardCount);

    protected:
        static constexpr uint8_t kDiscardPileSizeBits = 6;
        static constexpr uint16_t kDiscardPileContentBits = ::game_data::card_data::kTotalCards * ::game_data::card_data::kCardIDBits;

        static constexpr uint16_t kDiscardPileSizeOffset = 0;
        static constexpr uint16_t kDiscardPileContentOffset = kDiscardPileSizeOffset + kDiscardPileSizeBits;

        using DiscardPileData = std::array<uint8_t, (7 + kDiscardPileSizeBits + card_data::kCardIDBits * ::game_data::card_data::kTotalCards) / 8>;

        DiscardPileData discardPileData = {};     
};
}
}