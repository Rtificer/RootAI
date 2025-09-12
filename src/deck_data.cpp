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
[[nodiscard]] consteval Deck<deckType>::CardPileData Deck<deckType>::initialize_pile() const
{
    CardPileData data{};
    uint16_t bitPos = 0;
    set_deck_size_bits<0, kPileSizeBits>(data, bitPos);
    set_card_ids<0, card_data::kTotalCards>(data, bitPos);
    return data;
}

template <DeckType deckType>
[[nodiscard]] consteval std::expected<void, pile_data::PileError> Deck<deckType>::shuffle()
{
    r123::Threefry2x32_R<12> rng;
    const auto fetchResult = get_pile_contents();
    [[unlikely]] if (!fetchResult)
        return std::unexpected(fetchResult.error);

    auto& pile = fetchResult.value();

    for (uint8_t i = pile.size() - 1; i > 0; --i) {
        ++ctr[0];
        auto rand = rng(ctr, key);
        const uint8_t r = rand[0] % (i + 1);

        std::swap(pile[i], pile[r]);       
    }

    return set_pile_contents(pile);
}

template <DeckType deckType>
[[nodiscard]] inline consteval std::expected<void, pile_data::PileError> Deck<deckType>::turnover_discard(discard_pile_data::DiscardPile &discardPile) 
{
    auto getDiscardResult = discardPile.get_pile_contents();
    [[unlikely]] if (!getDiscardResult)
        return std::unexpected(getDiscardResult.error());

    auto& contents = getDiscardResult.value();

    auto deckSetResult = set_pile_contents(contents);
    if (!deckSetResult)
        return std::unexpected(deckSetResult.error());

    return discardPile.set_pile_size(0);
}


} // deck_data
} // game_data