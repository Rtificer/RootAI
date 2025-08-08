#include "../include/factions_data.hpp"

namespace game_data
{
namespace faction_data
{

template <typename FactionType, bool isAI>
[[nodiscard]] inline ExpandedScore Faction<FactionType, isAI>::get_score() const {
    return read_bits<ExpandedScore, kScoreOffset, kScoreBits>();
}

template <typename FactionType, bool isAI>
inline void Faction<FactionType, isAI>::set_score(ExpandedScore newScore) {
    write_bits<ExpandedScore, kScoreOffset, kScoreBits>(newScore);
}


template <typename FactionType, bool isAI>
[[nodiscard]] inline uint8_t Faction<FactionType, isAI>::get_hand_size() const {
    return read_bits<uint8_t, kHandSizeOffset, kHandSizeBits>();
}

template <typename FactionType, bool isAI>
inline void Faction<FactionType, isAI>::set_hand_size(uint8_t newSize) {
    // [[unlikely]] if (newSize > kMaxHandSize) {
    //     throw std::invalid_argument("New hand size cannot exceed maximum hand size");
    // }

    write_bits<uint8_t, kHandSizeOffset, kHandSizeBits>(newSize);
}

template <typename FactionType, bool isAI>
template <uint8_t newSize>
inline void Faction<FactionType, isAI>::set_hand_size() {
    static_assert(newSize <= kMaxHandSize, "New hand size cannot exceed maximum hand size");
    
    write_bits<uint8_t, kHandSizeOffset, kHandSizeBits>(newSize);
}

template <typename FactionType, bool isAI>
[[nodiscard]] std::vector<card_data::CardID> Faction<FactionType, isAI>::get_hand_contents() const {
    const uint8_t handSize = get_hand_size();
    if (handSize == 0) {
        return {};
    } else if (handSize == 1) {
        // Fast path for a single card
        return {read_bits<card_data::CardID, kHandContentOffset, card_data::kCardIDBits>()};
    } else [[likely]] if (handSize <= kMaxHandSize) {
        std::vector<card_data::CardID> result;
        // Compile-time dispatch for all possible hand sizes (starting from 2)
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            // bool handled = false;
            (((handSize == (Ns + 2)) ?
                ([&] {
                    constexpr size_t N = Ns + 2;
                    std::array<card_data::CardID, N> tempArray =
                        read_bits<std::array<card_data::CardID, N>, kHandContentOffset, card_data::kCardIDBits>();
                    result.assign(tempArray.begin(), tempArray.end());
                    // handled = true;
                }(), void()) : void()), ...);
            // [[unlikely]] if (!handled) {
            //     throw std::invalid_argument("Hand size exceeds maximum");
            // }
        };
        dispatch(std::make_index_sequence<kMaxHandSize - 1>{});
        return result;
    } /*else {
        throw std::invalid_argument("Hand size exceeds maximum");
    }*/
};

template <typename FactionType, bool isAI>
void Faction<FactionType, isAI>::set_hand_contents(const std::vector<card_data::CardID> &newHand) {
    uint8_t newHandSize = newHand.size();

    if (newHandSize == 0) {
        set_hand_size<0>();
    } else if (newHandSize == 1) {
        // Fast path for a single card
        write_bits<card_data::CardID, kHandContentOffset, card_data::kCardIDBits>(newHand[0]);
        set_hand_size<1>();
    } else [[likely]] if (newHandSize <= kMaxHandSize) {
        // Compile-time dispatch for all possible hand sizes (starting from 2)
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            // bool handled = false;
            (((newHandSize == (Ns + 2)) ?
                ([&] {
                    constexpr size_t N = Ns + 2;
                    std::array<card_data::CardID, N> tempArray{};
                    std::copy_n(newHand.begin(), N, tempArray.begin());
                    write_bits<std::array<card_data::CardID, N>, kHandContentOffset, card_data::kCardIDBits>(tempArray);
                    set_hand_size<N>();
                    // handled = true;
                }(), void()) : void()), ...);
            // [[unlikely]] if (!handled) {
            //     throw std::invalid_argument("Hand size exceeds maximum");
            // }
        };
        dispatch(std::make_index_sequence<kMaxHandSize - 1>{});
    } /*else {
        throw std::invalid_argument("Hand size exceeds maximum");
    }*/
}

template <typename FactionType, bool isAI>
[[nodiscard]] std::vector<card_data::CardID> Faction<FactionType, isAI>::get_cards_in_hand(const std::vector<uint8_t> &desiredCardIndices) const {
    // [[unlikely]] if (desiredCardIndices.empty())
    //     throw std::invalid_argument("Cannot get 0 cards");

    // std::vector<uint8_t> sortedIndices = desiredCardIndices;
    // std::sort(sortedIndices.begin(), sortedIndices.end());

    // [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end()) {
    //     throw std::invalid_argument("Duplicate indices are not allowed");
    // }

    // const uint8_t currentHandSize = get_hand_size();

    // [[unlikely]] if (sortedIndices.back() >= currentHandSize) {
    //     throw std::invalid_argument("Attempted to set invalid card");
    // }

    std::vector<card_data::CardID> result;
    result.reserve(desiredCardIndices.size());
    for (uint8_t idx : desiredCardIndices) {
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            // bool handled = false;
            (((idx == Ns) ?
                ([&] {
                    result.push_back(read_bits<card_data::CardID, Ns * card_data::kCardIDBits + kHandContentOffset, card_data::kCardIDBits>());
                    // handled = true;
                }(), void()) : void()), ...);
            // [[unlikely]] if (!handled) {
            //     throw std::invalid_argument("Desired card index exceeded maximum card index (dispatch)");
            // }
        };
        dispatch(std::make_index_sequence<kMaxHandSize>{});
    }
    return result;
}

template <typename FactionType, bool isAI>
void Faction<FactionType, isAI>::set_cards_in_hand(const std::vector<std::pair<uint8_t, card_data::CardID>> &newIndexCardPairs) {

    // [[unlikely]] if (newIndexCardPairs.empty())
    //     throw std::invalid_argument("Cannot set 0 cards");

    // std::vector<uint8_t> indices;
    // indices.reserve(newIndexCardPairs.size());
    // for (const auto& pair : newIndexCardPairs)
    //     indices.push_back(pair.first);

    // std::sort(indices.begin(), indices.end());
    // [[unlikely]] if (std::adjacent_find(indices.begin(), indices.end()) != indices.end()) {
    //     throw std::invalid_argument("Duplicate indices are not allowed");
    // }

    // uint8_t highestIndex = indices.back();

    for (const auto& pair : newIndexCardPairs) {
        // Compile-time dispatch for each possible index
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            // bool handled = false;
            (((pair.first == Ns) ?
                ([&] {
                    write_bits<card_data::CardID, Ns * card_data::kCardIDBits + kHandContentOffset, card_data::kCardIDBits>(pair.second);
                    // handled = true;
                }(), void()) : void()), ...);
            // [[unlikely]] if (!handled) {
            //     throw std::invalid_argument("Desired card index exceeded maximum card index");
            // }
        };
        dispatch(std::make_index_sequence<kMaxHandSize>{});
    }
}

template <typename FactionType, bool isAI>
void Faction<FactionType, isAI>::add_cards_to_hand(const std::vector<card_data::CardID> &newCards) {
    
    const uint8_t newCardsCount = newCards.size();
    const uint8_t oldHandSize = get_hand_size();
    // [[unlikely]] if (newCardsCount + oldHandSize <= kMaxHandSize)
    //     throw std::invalid_argument("Attempted to add zero cards to hand");

    auto add_cards_universal = [&](auto N) {
        std::array<card_data::CardID, N> tempArray{};
        std::copy_n(newCards.begin(), N, tempArray.begin());
        write_bits<std::array<card_data::CardID, N>,
            oldHandSize * card_data::kCardIDBits + kHandContentOffset, card_data::kCardIDBits>(tempArray);
        set_hand_size<oldHandSize + N>();
    };

    if (newCardsCount == 1)
    {
        write_bits<card_data::CardID, card_data::kCardIDBits * oldHandSize + kHandContentOffset, card_data::kCardIDBits>(newCards[0]);
        set_hand_size<oldHandSize + 1>();
    }
    else
    {
        // Use a compile-time unrolled dispatch to handle all possible newCardsCount values
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>)
        {
            // bool handled = false;
            // For each possible value Ns (from 0 to kMaxHandSize), check if newCardsCount matches
            // If it does, call add_cards_universal<N> with the corresponding compile-time constant
            ((newCardsCount == (Ns + 2) ? 
                ([&] {
                    constexpr size_t N = Ns + 2;
                    std::array<card_data::CardID, N> tempArray{};
                    std::copy_n(newCards.begin(), N, tempArray.begin());
                    write_bits<std::array<card_data::CardID, N>,
                        oldHandSize * card_data::kCardIDBits + kHandContentOffset, card_data::kCardIDBits>(tempArray);
                    set_hand_size<oldHandSize + N>();                  
                    // handled = true;
                }(), void()) : void()), ...);

            // If no match was found (should not happen), throw an exception
            // [[unlikely]] if (!handled)
            // {
            //     throw std::invalid_argument("Attempted to add zero cards to hand");
            // }
        };
        // Generate the sequence [0, 1, ..., kMaxHandSize] at compile time and call dispatch with it
        dispatch(std::make_index_sequence<kMaxHandSize - 1>{});
    }
}

template <typename FactionType, bool isAI>
void Faction<FactionType, isAI>::remove_cards_from_hand(const std::vector<uint8_t> &indices) {
    // [[unlikely]] if (indices.empty())
    //     throw std::invalid_argument("Cannot remove 0 cards");

    std::vector<uint8_t> sortedIndices = indices;
    std::sort(sortedIndices.begin(), sortedIndices.end());
    // [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end()) {
    //     throw std::invalid_argument("Duplicate indices are not allowed");
    // }

    const uint8_t currentHandSize = get_hand_size();

    // [[unlikely]] if (sortedIndices.back() >= currentHandSize) {
    //     throw std::invalid_argument("Attempted to set invalid card");
    // }

    const std::vector<card_data::CardID> handContentsVector = get_hand_contents();

    std::vector<card_data::CardID> newHand;
    newHand.reserve(currentHandSize - sortedIndices.size());
    size_t removeIdx = 0;
    for (uint8_t i = 0; i < currentHandSize; ++i) {
        if (removeIdx < sortedIndices.size() && i == sortedIndices[removeIdx]) {
            ++removeIdx;
            continue;
        }
        newHand.push_back(handContentsVector[i]);
    }

    set_hand_contents(newHand);
}

template <typename FactionType, bool isAI>
[[nodiscard]] inline uint8_t Faction<FactionType, isAI>::get_remaining_pawn_count() const requires HasPawns<FactionType> {
    return read_bits<uint8_t, kPawnOffset, FactionType::kPawnBits>();
}

template <typename FactionType, bool isAI>
inline void Faction<FactionType, isAI>::set_remaining_pawn_count(uint8_t newCount) requires HasPawns<FactionType> {
    write_bits<uint8_t, kPawnOffset, FactionType::kPawnBits>(newCount);
}

template <typename FactionType, bool isAI>
template <::game_data::deck_data::DeckType deckType>
inline void Faction<FactionType, isAI>::draw_cards(::game_data::deck_data::Deck<deckType> &deck, /*::game_data::discard_pile_data::DiscardPile &discard_pile, std::mt19937& engine,*/ uint8_t count)
{
    // if (count > deck.get_deck_size()) {
    //     auto discardContents = discard_pile.get_pile_contents();
    //     std::shuffle(discardContents.value().begin(), discardContents.value().end(), engine);
    //     deck.add_cards_to_pile(std::move(discardContents));
    //     discard_pile.set_pile_size<0>();
    // }
    auto newCards = deck.get_cards_in_pile(0, count - 1);
    deck.pop_cards_from_deck(count);
    add_cards_to_hand(std::move(newCards));
}

template <typename FactionType, bool isAI>
inline void Faction<FactionType, isAI>::discard_card_from_hand(::game_data::discard_pile_data::DiscardPile &discard_pile, uint8_t cardIndex) 
{
    discard_pile.add_cards_to_pile(get_cards_in_hand(cardIndex));
    remove_cards_from_hand(cardIndex);
}


} // faction_data
} // game_data