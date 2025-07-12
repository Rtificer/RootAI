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

        auto unroll = [&](auto... i)
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
        
        auto unroll = [&](auto... i)
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
    static_assert(newSize > 0 && newSize < 9, "Attempted to set invalid deck size value");

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
    } else if (deckSize <= kMaxDeckSize) {
        std::vector<::game_data::card_data::CardID> result;
        // Compile-time dispatch for all possible deck sizes
        auto dispatch = [&](auto... Ns) {
            bool handled = false;
            ((deckSize == Ns ?
                ([&] {
                    constexpr size_t N = decltype(Ns)::value;
                    std::array<::game_data::card_data::CardID, N> tempArray =
                        read_bits<std::array<::game_data::card_data::CardID, N>, kDeckContentOffset, ::game_data::card_data::kCardIDBits>();
                    result.assign(tempArray.begin(), tempArray.end());
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("Deck size exceeds maximum");
            }
        };
        dispatch(std::make_integer_sequence<size_t, kMaxDeckSize + 1>{});
        return result;
    } else {
        throw std::invalid_argument("Deck size exceeds maximum");
    }
}

template <IsValidDeck DeckType>
void Deck<DeckType>::set_deck_contents(const std::vector<::game_data::card_data::CardID> &newDeck) {
    // Helper lambda to write N cards to deck and set the deck size at compile time
    auto set_deck_universal = [&](auto N) {
        std::array<::game_data::card_data::CardID, N> tempArray{};
        std::copy_n(newDeck.begin(), N, tempArray.begin());
        write_bits<std::array<::game_data::card_data::CardID, N>, kDeckContentOffset, ::game_data::card_data::kCardIDBits>(tempArray);
        set_deck_size<N>();
    };

    uint8_t newDeckSize = newDeck.size();

    [[unlikely]] if (newDeckSize == 0) {
        set_deck_size<0>();
    } else [[unlikely]] if (newDeckSize == 1) {
        // Fast path for a single card
        write_bits<::game_data::card_data::CardID, kDeckContentOffset, ::game_data::card_data::kCardIDBits>(newDeck[0]);
        set_deck_size<1>();
    } else [[likely]] if (newDeckSize <= kMaxDeckSize) {
        // Use a compile-time unrolled dispatch to handle all possible deck sizes
        auto dispatch = [&](auto... Ns) {
            bool handled = false;
            // For each possible value Ns (from 0 to kMaxDeckSize), check if newDeck.size() matches
            // If it does, call set_deck_universal<N> with the corresponding compile-time constant
            ((newDeckSize == Ns ?
                (set_deck_universal(std::integral_constant<size_t, Ns>{}), handled = true)
                : void()), ...); // Fold expression to expand for all Ns

            // If no match was found (should not happen), throw an exception
            [[unlikely]] if (!handled) {
                throw std::invalid_argument("Deck size exceeds maximum");
            }
        };
        // Generate the sequence [0, 1, ..., kMaxDeckSize] at compile time and call dispatch with it
        dispatch(std::make_integer_sequence<size_t, kMaxDeckSize + 1>{});
    }
    else {
        throw std::invalid_argument("Deck size exceeds maximum");
    }
}

template <IsValidDeck DeckType>
[[nodiscard]] std::vector<::game_data::card_data::CardID> Deck<DeckType>::get_cards_in_deck(const std::vector<uint8_t> &desiredCardIndices) const {
    [[unlikely]] if (std::any_of(desiredCardIndices.begin(), desiredCardIndices.end(),
        [](uint8_t index)
        { return index >= kMaxDeckSize; }))
    {
        throw std::invalid_argument("Desired card index exceeded maximum card index");
    }

    std::vector<::game_data::card_data::CardID> result;
    result.reserve(desiredCardIndices.size());
    for (uint8_t i = 0; i < desiredCardIndices.size(); ++i) {
        result.push_back(read_bits<::game_data::card_data::CardID, i * ::game_data::card_data::kCardIDBits + kDeckContentOffset, ::game_data::card_data::kCardIDBits>());
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
    
    [[unlikely]] if (totalIndices == 1) {
        // Fast path for a single card
        return {
            read_bits<::game_data::card_data::CardID, startIndex * ::game_data::card_data::kCardIDBits + kDeckContentOffset, ::game_data::card_data::kCardIDBits>() };
    } else {
        std::vector<::game_data::card_data::CardID> result;
        // Compile-time dispatch for all possible deck sizes
        auto dispatch = [&](auto... Ns) {
            bool handled = false;
            ((totalIndices == Ns ?
                ([&] {
                    constexpr size_t N = decltype(Ns)::value;
                    std::array<::game_data::card_data::CardID, N> tempArray =
                        read_bits<std::array<::game_data::card_data::CardID, N>, startIndex * ::game_data::card_data::kCardIDBits + kDeckContentOffset, ::game_data::card_data::kCardIDBits>();
                    result.assign(tempArray.begin(), tempArray.end());
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("Index exceeded maximum card index");
            }
        };
        dispatch(std::make_integer_sequence<size_t, totalIndices + 1>{});
        return result;
}

template <IsValidDeck DeckType>
void Deck<DeckType>::set_cards_in_deck(const std::vector<std::pair<uint8_t, ::game_data::card_data::CardID>> &newIndexCardPairs) {
    [[unlikely]] if (std::any_of(newIndexCardPairs.begin(), newIndexCardPairs.end(),
        [](const std::pair<uint8_t, ::game_data::card_data::CardID> &pair)
        { return pair.first >= kMaxDeckSize; }))
    {
        throw std::invalid_argument("Desired card index exceeded maximum card index");
    }

    [[unlikely]] if (std::any_of(newIndexCardPairs.begin(), newIndexCardPairs.end(),
        [](const std::pair<uint8_t, ::game_data::card_data::CardID> &pair)
        { return pair.second == ::game_data::card_data::CardID::kNotACard; }))
    {
        throw std::invalid_argument("Attempted to Set Invalid card");
    }

    uint8_t highestIndex = 0;
    for (uint8_t i = 0; i < newIndexCardPairs.size(); ++i)
    {
        if (newIndexCardPairs[i].first > highestIndex)
            highestIndex = newIndexCardPairs[i].first;

        write_bits<::game_data::card_data::CardID, newIndexCardPairs[i].first * ::game_data::card_data::kCardIDBits + kDeckContentOffset, ::game_data::card_data::kCardIDBits>(newIndexCardPairs[i].second);
    }

    set_deck_size<highestIndex + 1>();
}

template<IsValidDeck DeckType>
void Deck<DeckType>::add_cards_to_deck(const std::vector<::game_data::card_data::CardID> &newCards) {
    const uint8_t newCardsCount = newCards.size();
    const uint8_t oldDeckSize = get_deck_size();
    [[unlikely]] if (newCardsCount + oldDeckSize > kMaxDeckSize)
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
    else [[likely]] if (newCardsCount <= kMaxDeckSize)
    {
        // Use a compile-time unrolled dispatch to handle all possible newCardsCount values
        auto dispatch = [&](auto... Ns)
        {
            bool handled = false;
            // For each possible value Ns (from 0 to kMaxDeckSize), check if newCardsCount matches
            // If it does, call add_cards_universal<N> with the corresponding compile-time constant
            ((newCardsCount == Ns ? 
                (add_cards_universal(std::integral_constant<size_t, Ns>{}), handled = true) // Call for matching N
                : void()), ...); // Fold expression to exp`and for all Ns

            // If no match was found (should not happen), throw an exception
            [[unlikely]] if (!handled)
            {
                throw std::invalid_argument("Attempted to add zero cards to deck");
            }
        };
        // Generate the sequence [0, 1, ..., kMaxDeckSize] at compile time and call dispatch with it
        dispatch(std::make_integer_sequence<size_t, kMaxDeckSize + 1>{});
    }
    else
    {
        throw std::invalid_argument("Attempted to add zero cards to deck");
    }
}

template<IsValidDeck DeckType>
void Deck<DeckType>::remove_cards_from_deck(const std::vector<uint8_t> &indices) {
    // Get the current deck size
    const uint8_t currentDeckSize = get_deck_size();

    // Validate that all indices are within bounds
    [[unlikely]] if (std::any_of(indices.begin(), indices.end(),
        [currentDeckSize](uint8_t index)
        { return index >= currentDeckSize; }))
    {
        throw std::invalid_argument("Attempted to Set Invalid card");
    }

    // Get the current deck contents
    std::vector<::game_data::card_data::CardID> deckContentsVector = get_deck_contents();

    // Create a mask to mark which indices should be removed
    std::vector<bool> removeMask(currentDeckSize, false);
    for (uint8_t index : indices) {
        removeMask[index] = true;
    }

    // Build the new deck by skipping cards marked for removal
    std::vector<::game_data::card_data::CardID> newDeck;
    newDeck.reserve(currentDeckSize - indices.size());
    for (uint8_t i = 0; i < currentDeckSize; ++i) {
        if (!removeMask[i])
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
} // game_dataresult