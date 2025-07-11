#include "../include/deck_data.hpp"

namespace game_data
{
namespace deck_data
{
    template <IsValidDeck DeckType>
    constexpr Deck<DeckType>::DeckData Deck<DeckType>::initialize_deck()
    {
        DeckData data{};
        // Set deck size bits
        uint16_t bitPos = 0;
        uint8_t deckSize = kMaxDeckSize;
        for (uint8_t bit = 0; bit < kDeckSizeBits; ++bit, ++bitPos)
        {
            if (deckSize & (1 << bit))
                data[bitPos / 8] |= (1 << (bitPos % 8));
        }
        // Set card IDs
        for (uint8_t cardIdx = 0; cardIdx < kMaxDeckSize; ++cardIdx)
        {
            uint8_t cardID = static_cast<uint8_t>(DeckType::kStartingCards[cardIdx]);
            for (uint8_t bit = 0; bit < card_data::kCardIDBits; ++bit, ++bitPos)
            {
                if (cardID & (1 << bit))
                    data[bitPos / 8] |= (1 << (bitPos % 8));
            }
        }
        return data;
    }
}
}