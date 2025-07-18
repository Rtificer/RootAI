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
    uint8_t deckSize = ::game_data::card_data::kTotalCards;
    for (uint8_t bit = 0; bit < kDeckSizeBits; ++bit, ++bitPos)
    {
        if (deckSize & (1 << bit))
            data[bitPos / 8] |= (1 << (bitPos % 8));
    }
    // Set card IDs
    for (uint8_t cardIdx = 0; cardIdx < ::game_data::card_data::kTotalCards; ++cardIdx)
    {
        uint8_t cardID = static_cast<uint8_t>(DeckType::kStartingCards[cardIdx]);
        for (uint8_t bit = 0; bit < ::game_data::card_data::kCardIDBits; ++bit, ++bitPos)
        {
            if (cardID & (1 << bit))
                data[bitPos / 8] |= (1 << (bitPos % 8));
        }
    }
    return data;
}

// Copy pasted from factions_data, and modified for only uint8_t -- fix later (or not)
template <IsValidDeck DeckType>
template<::game_data::IsUint8OrEnumUint8 OutputType, uint16_t shift, uint8_t width>
[[nodiscard]] OutputType Deck<DeckType>::read_bits() const {
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= 8, "Width cannot exceed one byte");

    // THIS IS ONLY COMPILE TIME CHECKING PLEASE DON'T BE STUPID, FORGET, AND RUIN IT
    constexpr uint8_t lastByte = (shift + width - 1) / 8;
    static_assert(lastByte < sizeof(deckData), "Not enough data to read requested bits");

    constexpr uint8_t byteIndex = shift / 8;
    constexpr uint8_t bitOffset = shift % 8;

    OutputType value = 0;

    // Fast path: byte-aligned reads can use direct access when width is exactly one byte
    if constexpr (bitOffset == 0 && width == 8)
    {
        value = static_cast<OutputType>(deckData[byteIndex]);
    }
    else
    {
        uint8_t temp = deckData[byteIndex];

        // Handle case where we need bits from a second byte
        if constexpr (bitOffset + width > 8)
            temp |= deckData[byteIndex + 1] << (8 - bitOffset);

        temp >>= bitOffset;

        if constexpr (width < 8) {
            constexpr uint8_t mask = (1U << width) - 1;
            temp &= mask;
        }

        value = static_cast<OutputType>(temp);
    }
    return value;
}

// Copy pasted from factions_data -- fix later (or not)
template <IsValidDeck DeckType>
template<::game_data::IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
[[nodiscard]] OutputType Deck<DeckType>::read_bits() const {
    static_assert(elementWidth > 0, "Element width cannot be zero");
    static_assert(elementWidth <= 8, "Element width cannot exceed one byte");

    // Calculate the index of the last byte we'll need to access
    constexpr uint8_t lastByte = (shift + (sizeof(OutputType) - 1) * elementWidth) / 8;
    static_assert(lastByte < sizeof(deckData), "Not enough data to read requested bits");

    OutputType value = {};

    //maybe do SIMD but this is probably good enough
    // Fast path: contiguous, byte-aligned data can use direct memcpy
    if constexpr (elementWidth == 8 && (shift % 8 == 0))
    {
        std::memcpy(&value, &deckData[shift / 8], sizeof(OutputType));
    }
    else
    {
        // General path for non-aligned or partial byte reads (unrolled at compile time)

        auto unroll = [&]<size_t... i>(std::index_sequence<i...>)
        {
            (([&] {
                    // Calculate the starting bit position for this element
                    // and the corresponding byte index
                    // (byteIndex = (shift + i * elementWidth) / 8)
                        constexpr uint16_t bitPos = shift + i * elementWidth;
                    constexpr uint8_t byteIndex = bitPos / 8;
                    constexpr uint8_t bitOffset = bitPos % 8;

                    // Always read the current byte
                    uint8_t byteValue = deckData[byteIndex];

                    // If the bits for this element cross a byte boundary,
                    // also read the next byte and combine
                    if constexpr (bitOffset + elementWidth > 8)
                    {
                        byteValue |= deckData[byteIndex + 1] << (8 - bitOffset);
                    }

                    // Shift right to align the desired bits to LSB
                    byteValue >>= bitOffset;

                    // Mask to keep only the bits we're interested in
                    if constexpr (elementWidth < 8) {
                        constexpr uint8_t mask = (1U << elementWidth) - 1;
                        byteValue &= mask;
                    }
                    
                    value[i] = static_cast<OutputType::value_type>(byteValue); 
                }()),
            // Fold expression repeats this for all indices
            ...);
        };

        // Execute the unrolled loop using compile-time index sequence
        unroll(std::make_index_sequence<sizeof(OutputType)>{});
    }
    return value;
}

// Copy pasted from factions_data, and modified for only uint8_t -- fix later (or not)
template <IsValidDeck DeckType>
template<::game_data::IsUint8OrEnumUint8 InputType, uint16_t shift, uint16_t width>
void Deck<DeckType>::write_bits(const InputType &value) {
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= 8, "Width cannot exceed one byte");

    // THIS IS ONLY COMPILE TIME CHECKING PLEASE DON'T BE STUPID, FORGET, AND RUIN IT
    constexpr uint8_t lastByte = (shift + width - 1) / 8;
    static_assert(lastByte < sizeof(deckData), "Not enough data to write requested bits");

    constexpr uint8_t byteIndex = shift / 8;
    constexpr uint8_t bitOffset = shift % 8;

    // Fast path: byte-aligned writes can use direct memcpy when width matches the type size exactly
    if constexpr (bitOffset == 0 && width == 8)
    {
        std::memcpy(&deckData[byteIndex], &value, sizeof(InputType));
    }
    else
    {
        uint8_t value_uint8 = static_cast<uint8_t>(value);
        constexpr uint8_t mask = (width < 8) ? ((1U << width) - 1) : 0xFF;
        value_uint8 &= mask;
        // Clear the target bits in the first byte
        deckData[byteIndex] &= ~(mask << bitOffset);
        // Set the bits from value
        deckData[byteIndex] |= (value_uint8 << bitOffset);

        // Handle case where we need bits from a second byte
        if constexpr (bitOffset + width > 8) {
            constexpr uint8_t nextMask = (mask >> (8 - bitOffset));
            deckData[byteIndex + 1] &= ~nextMask;
            deckData[byteIndex + 1] |= (value_uint8 >> (8 - bitOffset)) & nextMask;
        }
    }
}

template <IsValidDeck DeckType>
template<::game_data::IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
void Deck<DeckType>::write_bits(const InputType &value) {
    static_assert(elementWidth > 0, "Element width cannot be zero");
    static_assert(elementWidth <= 8, "Element width cannot exceed one byte");

    // Calculate the index of the last byte we'll need to access
    constexpr uint8_t lastByte = (shift + (sizeof(InputType) - 1) * elementWidth) / 8;
    static_assert(lastByte < sizeof(deckData), "Not enough data to write requested bits");

    //maybe do SIMD but this is probably good enough
    // Fast path: contiguous, byte-aligned data can use direct memcpy
    if constexpr (elementWidth == 8 && (shift % 8 == 0))
    {
        std::memcpy(&deckData[shift / 8], &value, sizeof(InputType));
    }
    else
    {
        // General path for non-aligned or partial byte reads (unrolled at compile time)
        
        auto unroll = [&]<size_t... i>(std::index_sequence<i...>)
        {
            (([&] {
                constexpr uint16_t bitPos = shift + i * elementWidth;
                constexpr uint8_t byteIndex = bitPos / 8;
                constexpr uint8_t bitOffset = bitPos % 8;

                uint8_t byteValue = static_cast<uint8_t>(value[i]);
                constexpr uint8_t mask = (elementWidth < 8) ? ((1U << elementWidth) - 1) : 0xFF;
                byteValue &= mask;
                // Clear the target bits in the first byte
                deckData[byteIndex] &= ~(mask << bitOffset);
                // Set the bits from value
                deckData[byteIndex] |= (byteValue << bitOffset);

                // Handle case where we need bits from a second byte
                if constexpr (bitOffset + elementWidth > 8) {
                    constexpr uint8_t nextMask = (mask >> (8 - bitOffset));
                    deckData[byteIndex + 1] &= ~nextMask;
                    deckData[byteIndex + 1] |= (byteValue >> (8 - bitOffset)) & nextMask;
                }                      
            }()),
            // Fold expression repeats this for all indices
            ...);
        };

        // Execute the unrolled loop using compile-time index sequence
        unroll(std::make_index_sequence<sizeof(InputType)>{});
    }
}

template <IsValidDeck DeckType>
[[nodiscard]] inline uint8_t Deck<DeckType>::get_deck_size() const {
    static_assert(kDeckSizeBits > 0 && kDeckSizeBits < 9, "Invalid kDeckSizeBits value");

    return read_bits<uint8_t, kDeckSizeOffset, kDeckSizeBits>();
}

template <IsValidDeck DeckType>
template <uint8_t newSize>
inline void Deck<DeckType>::set_deck_size() {
    static_assert(kDeckSizeBits > 0 && kDeckSizeBits < 9, "Attempted to set invalid deck size value");
    static_assert(newSize <= ::game_data::card_data::kTotalCards, "Deck size cannot be greater than the quantity of cards");

    write_bits<uint8_t, kDeckSizeOffset, kDeckSizeBits>(newSize);
}

template <IsValidDeck DeckType>
inline void Deck<DeckType>::set_deck_size(uint8_t newSize) {
    [[unlikely]] if (newSize > ::game_data::card_data::kTotalCards) {
        throw std::invalid_argument("New deck size cannot cannot be greater than the quantity of cards");
    }

    write_bits<uint8_t, kDeckSizeOffset, kDeckSizeBits>(newSize);
}

template <IsValidDeck DeckType>
[[nodiscard]] std::vector<::game_data::card_data::CardID> Deck<DeckType>::get_deck_contents() const {
    const uint8_t deckSize = get_deck_size();
    [[unlikely]] if (deckSize == 0) {
        return {};
    } else [[unlikely]] if (deckSize == 1) {
        // Fast path for a single card
        return {read_bits<::game_data::card_data::CardID, kDeckContentOffset, ::game_data::card_data::kCardIDBits>()};
    } else if (deckSize <= ::game_data::card_data::kTotalCards) {
        std::vector<::game_data::card_data::CardID> result;
        // Compile-time dispatch for all possible deck sizes (starting from 2)
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            bool handled = false;
            (((deckSize == (Ns + 2)) ?
                ([&] {
                    constexpr size_t N = Ns + 2;
                    std::array<::game_data::card_data::CardID, N> tempArray =
                        read_bits<std::array<::game_data::card_data::CardID, N>, kDeckContentOffset, ::game_data::card_data::kCardIDBits>();
                    result.assign(tempArray.begin(), tempArray.end());
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("Deck size exceeds maximum");
            }
        };
        dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards - 1>{});
        return result;
    } else {
        throw std::invalid_argument("Deck size exceeds maximum");
    }
}

template <IsValidDeck DeckType>
void Deck<DeckType>::set_deck_contents(const std::vector<::game_data::card_data::CardID> &newDeck) {
    // Helper lambda to write N cards to deck and set the deck size at compile time
    auto set_deck_universal = [&](auto N) {
        constexpr size_t kN = decltype(N)::value;
        std::array<::game_data::card_data::CardID, kN> tempArray{};
        std::copy_n(newDeck.begin(), kN, tempArray.begin());
        write_bits<std::array<::game_data::card_data::CardID, kN>, kDeckContentOffset, ::game_data::card_data::kCardIDBits>(tempArray);
        set_deck_size<kN>();
    };

    uint8_t newDeckSize = newDeck.size();

    [[unlikely]] if (newDeckSize == 0) {
        set_deck_size<0>();
    } else [[unlikely]] if (newDeckSize == 1) {
        // Fast path for a single card
        write_bits<::game_data::card_data::CardID, kDeckContentOffset, ::game_data::card_data::kCardIDBits>(newDeck[0]);
        set_deck_size<1>();
    } else [[likely]] if (newDeckSize <= ::game_data::card_data::kTotalCards) {
        // Compile-time dispatch for all possible deck sizes (starting from 2)
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            bool handled = false;
            (((newDeckSize == (Ns + 2)) ?
                ([&] {
                    set_deck_universal(std::integral_constant<size_t, Ns + 2>{});
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("Deck size exceeds maximum");
            }
        };
        dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards - 1>{});
    } else {
        throw std::invalid_argument("Deck size exceeds maximum");
    }
}

template <IsValidDeck DeckType>
[[nodiscard]] std::vector<::game_data::card_data::CardID> Deck<DeckType>::get_cards_in_deck(const std::vector<uint8_t> &desiredCardIndices) const {
    [[unlikely]] if (desiredCardIndices.empty())
        throw std::invalid_argument("Cannot get 0 cards");

    std::vector<uint8_t> sortedIndices = desiredCardIndices;
    std::sort(sortedIndices.begin(), sortedIndices.end());

    [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end()) {
        throw std::invalid_argument("Duplicate indices are not allowed");
    }

    const uint8_t currentDeckSize = get_deck_size();

    [[unlikely]] if (sortedIndices.back() >= currentDeckSize) {
        throw std::invalid_argument("Attempted to set invalid card");
    }

    std::vector<::game_data::card_data::CardID> result;
    result.reserve(desiredCardIndices.size());
    for (uint8_t idx : desiredCardIndices) {
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            bool handled = false;
            (((idx == Ns) ?
                ([&] {
                    result.push_back(read_bits<::game_data::card_data::CardID, Ns * ::game_data::card_data::kCardIDBits + kDeckContentOffset, ::game_data::card_data::kCardIDBits>());
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("Desired card index exceeded maximum card index (dispatch)");
            }
        };
        dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards>{});
    }
    return result;
}

template <IsValidDeck DeckType>
[[nodiscard]] std::vector<::game_data::card_data::CardID> Deck<DeckType>::get_cards_in_deck(uint8_t startIndex, uint8_t endIndex) const {
    const uint8_t deckSize = get_deck_size();

    [[unlikely]] if (startIndex >= deckSize || endIndex >= deckSize)
        throw std::invalid_argument("Index exceeded maximum card index");

    [[unlikely]] if (startIndex >= endIndex)
        throw std::invalid_argument("Start index >= end index");

    const uint8_t totalIndices = endIndex - startIndex;
    
    // Use compile-time dispatch for each possible index, matching discard_pile_data.cpp
    std::vector<::game_data::card_data::CardID> result;
    result.reserve(totalIndices);
    auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
        ((Ns >= startIndex && Ns < endIndex ?
            result.push_back(read_bits<::game_data::card_data::CardID, Ns * ::game_data::card_data::kCardIDBits + kDeckContentOffset, ::game_data::card_data::kCardIDBits>())
            : void()), ...);
    };
    dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards>{});
    return result;
}

template <IsValidDeck DeckType>
void Deck<DeckType>::set_cards_in_deck(const std::vector<std::pair<uint8_t, ::game_data::card_data::CardID>> &newIndexCardPairs) {

    [[unlikely]] if (newIndexCardPairs.empty())
        throw std::invalid_argument("Cannot set 0 cards");

    std::vector<uint8_t> indices;
    indices.reserve(newIndexCardPairs.size());
    for (const auto& pair : newIndexCardPairs) {
        indices.push_back(pair.first);
        if (pair.first >= ::game_data::card_data::kTotalCards)
            throw std::invalid_argument("Desired card index exceeded maximum card index");
        if (pair.second == ::game_data::card_data::CardID::kNotACard)
            throw std::invalid_argument("Attempted to Set Invalid card");
    }

    std::sort(indices.begin(), indices.end());
    [[unlikely]] if (std::adjacent_find(indices.begin(), indices.end()) != indices.end()) {
        throw std::invalid_argument("Duplicate indices are not allowed");
    }

    uint8_t highestIndex = indices.back();

    for (const auto& pair : newIndexCardPairs) {
        // Compile-time dispatch for each possible index
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            bool handled = false;
            (((pair.first == Ns) ?
                ([&] {
                    write_bits<::game_data::card_data::CardID, Ns * ::game_data::card_data::kCardIDBits + kDeckContentOffset, ::game_data::card_data::kCardIDBits>(pair.second);
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("Desired card index exceeded maximum card index (dispatch)");
            }
        };
        dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards>{});
    }

    set_deck_size(highestIndex + 1);
}

template<IsValidDeck DeckType>
void Deck<DeckType>::add_cards_to_deck(const std::vector<::game_data::card_data::CardID> &newCards) {
    const uint8_t newCardsCount = newCards.size();
    const uint8_t oldDeckSize = get_deck_size();
    [[unlikely]] if (newCardsCount + oldDeckSize > ::game_data::card_data::kTotalCards)
        throw std::invalid_argument("Attempted to add cards above the deck size limit");

    [[unlikely]] if (std::any_of(newCards.begin(), newCards.end(),
        [](::game_data::card_data::CardID card)
        { return card == ::game_data::card_data::CardID::kNotACard; }))
    {
        throw std::invalid_argument("Attempted to Set Invalid card");
    }

    auto add_cards_universal = [&](auto N) {
        std::array<::game_data::card_data::CardID, N> tempArray{};
        std::copy_n(newCards.begin(), N, tempArray.begin());
        write_bits<std::array<::game_data::card_data::CardID, N>,
            oldDeckSize * ::game_data::card_data::kCardIDBits + kDeckContentOffset, ::game_data::card_data::kCardIDBits>(tempArray);
        set_deck_size<oldDeckSize + N>();
    };

    if (newCardsCount == 1)
    {
        write_bits<::game_data::card_data::CardID, ::game_data::card_data::kCardIDBits * oldDeckSize + kDeckContentOffset, ::game_data::card_data::kCardIDBits>(newCards[0]);
        set_deck_size<oldDeckSize + 1>();
    }
    else [[likely]] if (newCardsCount <= ::game_data::card_data::kTotalCards)
    {
        // Use a compile-time unrolled dispatch to handle all possible newCardsCount values (starting from 2)
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>)
        {
            bool handled = false;
            (((newCardsCount == (Ns + 2)) ?
                (add_cards_universal(std::integral_constant<size_t, Ns + 2>{}), handled = true)
                : void()), ...);
            [[unlikely]] if (!handled)
            {
                throw std::invalid_argument("Attempted to add zero cards to deck");
            }
        };
        dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards - 1>{});
    }
    else
    {
        throw std::invalid_argument("Attempted to add zero cards to deck");
    }
}

template<IsValidDeck DeckType>
void Deck<DeckType>::remove_cards_from_deck(const std::vector<uint8_t> &indices) {
    [[unlikely]] if (indices.empty())
        throw std::invalid_argument("Cannot remove 0 cards");

    std::vector<uint8_t> sortedIndices = indices;
    std::sort(sortedIndices.begin(), sortedIndices.end());

    [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end()) {
        throw std::invalid_argument("Duplicate indices are not allowed");
    }

    const uint8_t currentDeckSize = get_deck_size();

    [[unlikely]] if (sortedIndices.back() >= currentDeckSize) {
        throw std::invalid_argument("Attempted to set invalid card");
    }

    std::vector<::game_data::card_data::CardID> deckContentsVector = get_deck_contents();

    std::vector<::game_data::card_data::CardID> newDeck;
    newDeck.reserve(currentDeckSize - sortedIndices.size());
    size_t removeIdx = 0;
    for (uint8_t i = 0; i < currentDeckSize; ++i) {
        if (removeIdx < sortedIndices.size() && i == sortedIndices[removeIdx]) {
            ++removeIdx;
            continue;
        }
        newDeck.push_back(deckContentsVector[i]);
    }

    set_deck_contents(newDeck);
}

template <IsValidDeck DeckType>
inline void Deck<DeckType>::pop_cards_from_deck(uint8_t count) {
    const uint8_t oldSize = get_deck_size();
    [[unlikely]] if (oldSize < count)
        throw std::invalid_argument("Attempted to pop more cards then are left in the deck"); 

    set_deck_size(oldSize - count);
}
} // deck_data
} // game_data