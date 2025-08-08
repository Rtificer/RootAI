#include "../include/deck_data.hpp"

namespace game_data
{
namespace deck_data
{

// Unroll a loop at compile time for deck size bits
// I always think these are so stupid
template <DeckType deckType>
template <size_t Bit, size_t Max>
consteval void Deck<deckType>::set_deck_size_bits(CardPileData& data, uint16_t& bitPos) const {
    if constexpr (Bit < Max) {
        if constexpr ((card_data::kTotalCards & (1 << Bit)) != 0)
            data[bitPos / 8] |= (1 << (bitPos % 8));
        ++bitPos;
        set_deck_size_bits<Bit + 1, Max>(data, bitPos);
    }
}

// Unroll a loop at compile time for card IDs
template <DeckType deckType>
template <size_t CardIdx, size_t Max>
consteval void Deck<deckType>::set_card_ids(CardPileData& data, uint16_t& bitPos) const {
    if constexpr (CardIdx < Max) {
        uint8_t cardID = (deckType == DeckType::kStandard)
            ? static_cast<uint8_t>(kStandardStartingCards[CardIdx])
            : static_cast<uint8_t>(kExilesAndPartisansStartingCards[CardIdx]);
        for (uint8_t bit = 0; bit < card_data::kCardIDBits; ++bit, ++bitPos)
            if (cardID & (1 << bit))
                data[bitPos / 8] |= (1 << (bitPos % 8));
        set_card_ids<CardIdx + 1, Max>(data, bitPos);
    }
}

template <DeckType deckType>
consteval Deck<deckType>::CardPileData Deck<deckType>::initialize_pile() const
{
    CardPileData data{};
    uint16_t bitPos = 0;
    set_deck_size_bits<0, kPileSizeBits>(data, bitPos);
    set_card_ids<0, card_data::kTotalCards>(data, bitPos);
    return data;
}
} // deck_data
} // game_data