#include "../include/factions_data.hpp"

namespace game_data
{
namespace faction_data
{
template <typename FactionType>
template <IsUnsignedIntegralOrEnum OutputType, uint16_t shift, uint16_t width>
[[nodiscard]] OutputType Faction<FactionType>::read_bits() const
{
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= sizeof(OutputType) * 8, "Width exceeds output type bit capacity");

    // THIS IS ONLY COMPILE TIME CHECKING PLEASE DON'T BE STUPID, FORGET, AND RUIN IT
    constexpr uint8_t lastByte = (shift + width - 1) / 8;
    static_assert(lastByte < sizeof(factionData), "Not enough data to read requested bits");

    constexpr uint8_t byteIndex = shift / 8;
    constexpr uint8_t bitOffset = shift % 8;

    OutputType value = 0;

    // Fast path: byte-aligned reads can use direct memcpy when width matches the type size exactly
    if constexpr (bitOffset == 0 && width == sizeof(OutputType) * 8)
    {
        std::memcpy(&value, &factionData[byteIndex], sizeof(OutputType));
    }
    else if constexpr (
        std::is_same_v<Clean_t<OutputType>, uint8_t> ||
        (std::is_enum_v<Clean_t<OutputType>> &&
        std::is_same_v<std::underlying_type_t<Clean_t<OutputType>>, uint8_t>)
    )
    {
        // Specialized for uint8_t or enum with uint8_t underlying type
        uint8_t temp = factionData[byteIndex];

        // Handle case where we need bits from a second byte
        if constexpr (bitOffset + width > 8)
            temp |= factionData[byteIndex + 1] << (8 - bitOffset);

        temp >>= bitOffset;

        if constexpr (width < 8) {
            constexpr uint8_t mask = (1U << width) - 1;
            temp &= mask;
        }

        value = static_cast<OutputType>(temp);
    }
    else
    {
        // Generic fallback (including other enum types)
        OutputType temp = 0;
        constexpr uint8_t bytesNeeded = (width + 7) / 8;

        std::memcpy(&temp, &factionData[byteIndex], bytesNeeded);
        if constexpr (std::endian::native == std::endian::big)
            temp = std::byteswap(temp);

        temp >>= bitOffset;

        if constexpr (width < sizeof(OutputType) * 8)
        {
            constexpr OutputType mask = (OutputType(1) << width) - 1;
            temp &= mask;
        }

        value = static_cast<OutputType>(temp);
    }
    return value;
}

template <typename FactionType>
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
        std::memcpy(&value, &factionData[shift / 8], sizeof(OutputType));
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
                    uint8_t byteValue = factionData[byteIndex];

                    // If the bits for this element cross a byte boundary,
                    // also read the next byte and combine
                    if constexpr (bitOffset + elementWidth > 8)
                    {
                        byteValue |= factionData[byteIndex + 1] << (8 - bitOffset);
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
};

template <typename FactionType>
template <IsUnsignedIntegralOrEnum InputType, uint16_t shift, uint16_t width>
void Faction<FactionType>::write_bits(const InputType &value) {
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= sizeof(InputType) * 8, "Width exceeds output type bit capacity");

    // THIS IS ONLY COMPILE TIME CHECKING PLEASE DON'T BE STUPID, FORGET, AND RUIN IT
    constexpr uint8_t lastByte = (shift + width - 1) / 8;
    static_assert(lastByte < sizeof(factionData), "Not enough data to write requested bits");

    constexpr uint8_t byteIndex = shift / 8;
    constexpr uint8_t bitOffset = shift % 8;

    // Fast path: byte-aligned writes can use direct memcpy when width matches the type size exactly
    if constexpr (bitOffset == 0 && width == sizeof(InputType) * 8)
    {
        std::memcpy(&factionData[byteIndex], &value, sizeof(InputType));
    }
    // Type-specific optimized paths
    else if constexpr (
        std::is_same_v<Clean_t<InputType>, uint8_t> ||
        (std::is_enum_v<Clean_t<InputType>> &&
        std::is_same_v<std::underlying_type_t<Clean_t<InputType>>, uint8_t>)
    )
    {
        // Specialized for uint8_t or enum with uint8_t underlying type
        uint8_t value_uint8 = static_cast<uint8_t>(value);
        constexpr uint8_t mask = (width < 8) ? ((1U << width) - 1) : 0xFF;
        value_uint8 &= mask;
        // Clear the target bits in the first byte
        factionData[byteIndex] &= ~(mask << bitOffset);
        // Set the bits from value
        factionData[byteIndex] |= (value_uint8 << bitOffset);

        // Handle case where we need bits from a second byte
        if constexpr (bitOffset + width > 8) {
            constexpr uint8_t nextMask = (mask >> (8 - bitOffset));
            factionData[byteIndex + 1] &= ~nextMask;
            factionData[byteIndex + 1] |= (value_uint8 >> (8 - bitOffset)) & nextMask;
        }
    }
    else
    {
        InputType mutableValue = value;
        if constexpr (std::endian::native == std::endian::big)
            mutableValue = std::byteswap(mutableValue);
        
        constexpr uint8_t bytesNeeded = (width + 7) / 8;

        InputType orig = 0;
        constexpr InputType mask = (width < bytesNeeded * 8) ? 
        ((~InputType(0)) >> (sizeof(InputType) * 8 - width)) : ~InputType(0);
        mutableValue &= mask;

        // Clear the target bits in the first four bytesstd::numeric_limits<InputType>::max();
        std::memcpy(&orig, &factionData[byteIndex], bytesNeeded);

        orig &= ~(mask << bitOffset); // clear target bits
        orig |= (mutableValue << bitOffset); // set new bits

        std::memcpy(&factionData[byteIndex], &orig, bytesNeeded);

        // Handle case where bits cross into an extra byte
        if constexpr (bitOffset + width > bytesNeeded * 8) {
            constexpr uint8_t nextMask = (mask >> (bytesNeeded * 8 - bitOffset));
            factionData[byteIndex + bytesNeeded] &= ~nextMask;
            factionData[byteIndex + bytesNeeded] |= (mutableValue >> (bytesNeeded * 8 - bitOffset)) & nextMask;
        }
    }
}

template <typename FactionType>
template <IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
void Faction<FactionType>::write_bits(const InputType &value) {
    static_assert(elementWidth > 0, "Element width cannot be zero");
    static_assert(elementWidth <= 8, "Element width cannot exceed one byte");

    // Calculate the index of the last byte we'll need to access
    constexpr uint8_t lastByte = (shift + (sizeof(InputType) - 1) * elementWidth) / 8;
    static_assert(lastByte < sizeof(factionData), "Not enough data to write requested bits");

    //maybe do SIMD but this is probably good enough
    // Fast path: contiguous, byte-aligned data can use direct memcpy
    if constexpr (elementWidth == 8 && (shift % 8 == 0))
    {
        std::memcpy(&factionData[shift / 8], &value, sizeof(InputType));
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
                factionData[byteIndex] &= ~(mask << bitOffset);
                // Set the bits from value
                factionData[byteIndex] |= (byteValue << bitOffset);

                // Handle case where we need bits from a second byte
                if constexpr (bitOffset + elementWidth > 8) {
                    constexpr uint8_t nextMask = (mask >> (8 - bitOffset));
                    factionData[byteIndex + 1] &= ~nextMask;
                    factionData[byteIndex + 1] |= (byteValue >> (8 - bitOffset)) & nextMask;
                }                      
            }()),
            // Fold expression repeats this for all indices
            ...);
        };

        // Execute the unrolled loop using compile-time index sequence
        unroll(std::make_index_sequence<sizeof(InputType)>{});
    }
};

template <typename FactionType>
[[nodiscard]] inline ExpandedScore Faction<FactionType>::get_score() const {
    return read_bits<ExpandedScore, kScoreOffset, kScoreBits>();
}

template <typename FactionType>
inline void Faction<FactionType>::set_score(ExpandedScore newScore) {
    write_bits<ExpandedScore, kScoreOffset, kScoreBits>(newScore);
}


template <typename FactionType>
[[nodiscard]] inline uint8_t Faction<FactionType>::get_hand_size() const {
    return read_bits<uint8_t, kHandSizeOffset, kHandSizeBits>();
}

template <typename FactionType>
inline void Faction<FactionType>::set_hand_size(uint8_t newSize) {
    [[unlikely]] if (newSize > kMaxHandSize) {
        throw std::invalid_argument("New hand size cannot exceed maximum hand size");
    }

    write_bits<uint8_t, kHandSizeOffset, kHandSizeBits>(newSize);
}

template <typename FactionType>
template <uint8_t newSize>
inline void Faction<FactionType>::set_hand_size() {
    static_assert(newSize <= kMaxHandSize, "New hand size cannot exceed maximum hand size");
    
    write_bits<uint8_t, kHandSizeOffset, kHandSizeBits>(newSize);
}

template <typename FactionType>
[[nodiscard]] std::vector<::game_data::card_data::CardID> Faction<FactionType>::get_hand_contents() const {
    const uint8_t handSize = get_hand_size();
    if (handSize == 0) {
        return {};
    } else if (handSize == 1) {
        // Fast path for a single card
        return {read_bits<::game_data::card_data::CardID, kHandContentOffset, ::game_data::card_data::kCardIDBits>()};
    } else if (handSize <= kMaxHandSize) {
        std::vector<::game_data::card_data::CardID> result;
        // Compile-time dispatch for all possible hand sizes
        auto dispatch = [&](auto... Ns) {
            bool handled = false;
            ((handSize == Ns ?
                ([&] {
                    constexpr size_t N = decltype(Ns)::value;
                    std::array<::game_data::card_data::CardID, N> tempArray =
                        read_bits<std::array<::game_data::card_data::CardID, N>, kHandContentOffset, ::game_data::card_data::kCardIDBits>();
                    result.assign(tempArray.begin(), tempArray.end());
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("Hand size exceeds maximum");
            }
        };
        dispatch(std::make_integer_sequence<size_t, kMaxHandSize + 1>{});
        return result;
    } else {
        throw std::invalid_argument("Hand size exceeds maximum");
    }
};

template <typename FactionType>
void Faction<FactionType>::set_hand_contents(const std::vector<::game_data::card_data::CardID> &newHand) {
    // Helper lambda to write N cards to hand and set the hand size at compile time
    auto set_hand_universal = [&](auto N) {
        std::array<::game_data::card_data::CardID, N> tempArray{};
        std::copy_n(newHand.begin(), N, tempArray.begin());
        write_bits<std::array<::game_data::card_data::CardID, N>, kHandContentOffset, ::game_data::card_data::kCardIDBits>(tempArray);
        set_hand_size<N>();
    };

    uint8_t newHandSize = newHand.size();

    if (newHandSize == 0) {
        set_hand_size<0>();
    } else if (newHandSize == 1) {
        // Fast path for a single card
        write_bits<::game_data::card_data::CardID, kHandContentOffset, ::game_data::card_data::kCardIDBits>(newHand[0]);
        set_hand_size<1>();
    } else if (newHandSize <= kMaxHandSize) {
        // Use a compile-time unrolled dispatch to handle all possible hand sizes
        auto dispatch = [&](auto... Ns) {
            bool handled = false;
            // For each possible value Ns (from 0 to kMaxHandSize), check if newHand.size() matches
            // If it does, call set_hand_universal<N> with the corresponding compile-time constant
            ((newHandSize == Ns ?
                (set_hand_universal(std::integral_constant<size_t, Ns>{}), handled = true)
                : void()), ...); // Fold expression to expand for all Ns

            // If no match was found (should not happen), throw an exception
            [[unlikely]] if (!handled) {
                throw std::invalid_argument("Hand size exceeds maximum");
            }
        };
        // Generate the sequence [0, 1, ..., kMaxHandSize] at compile time and call dispatch with it
        dispatch(std::make_integer_sequence<size_t, kMaxHandSize + 1>{});
    }
    else [[unlikely]] {
        throw std::invalid_argument("Hand size exceeds maximum");
    }
}

template <typename FactionType>
[[nodiscard]] std::vector<::game_data::card_data::CardID> Faction<FactionType>::get_cards_in_hand(const std::vector<uint8_t> &desiredCardIndices) const {
    [[unlikely]] if (std::any_of(desiredCardIndices.begin(), desiredCardIndices.end(),
        [](uint8_t index)
        { return index >= kMaxHandSize; }))
    {
        throw std::invalid_argument("Desired card index exceeded maximum card index");
    }

    std::vector<::game_data::card_data::CardID> result;
    result.reserve(desiredCardIndices.size());
    for (uint8_t i = 0; i < desiredCardIndices.size(); ++i) {
        result.push_back(read_bits<::game_data::card_data::CardID, i * ::game_data::card_data::kCardIDBits + kHandContentOffset, ::game_data::card_data::kCardIDBits>());
    }
    return result;
}

template <typename FactionType>
void Faction<FactionType>::set_cards_in_hand(const std::vector<std::pair<uint8_t, ::game_data::card_data::CardID>> &newIndexCardPairs) {
    [[unlikely]] if (std::any_of(newIndexCardPairs.begin(), newIndexCardPairs.end(),
        [](const std::pair<uint8_t, ::game_data::card_data::CardID> &pair)
        { return pair.first >= kMaxHandSize; }))
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

        write_bits<::game_data::card_data::CardID, newIndexCardPairs[i].first * ::game_data::card_data::kCardIDBits + kHandContentOffset, ::game_data::card_data::kCardIDBits>(newIndexCardPairs[i].second);
    }

    set_hand_size<highestIndex + 1>();
}

template <typename FactionType>
void Faction<FactionType>::add_cards_to_hand(const std::vector<::game_data::card_data::CardID> &newCards) {
    const uint8_t newCardsCount = newCards.size();
    const uint8_t oldHandSize = get_hand_size();
    [[unlikely]] if (newCardsCount + oldHandSize > kMaxHandSize)
        throw std::invalid_argument("Attempted to add cards above the hand size limit");

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
            oldHandSize * ::game_data::card_data::kCardIDBits + kHandContentOffset, ::game_data::card_data::kCardIDBits>(tempArray);
        set_hand_size<oldHandSize + N>();
    };

    if (newCardsCount == 1)
    {
        write_bits<::game_data::card_data::CardID, ::game_data::card_data::kCardIDBits * oldHandSize + kHandContentOffset, ::game_data::card_data::kCardIDBits>(newCards[0]);
        set_hand_size<oldHandSize + 1>();
    }
    else [[likely]] if (newCardsCount <= kMaxHandSize)
    {
        // Use a compile-time unrolled dispatch to handle all possible newCardsCount values
        auto dispatch = [&](auto... Ns)
        {
            bool handled = false;
            // For each possible value Ns (from 0 to kMaxHandSize), check if newCardsCount matches
            // If it does, call add_cards_universal<N> with the corresponding compile-time constant
            ((newCardsCount == Ns ? 
                (add_cards_universal(std::integral_constant<size_t, Ns>{}), handled = true) // Call for matching N
                : void()), ...); // Fold expression to exp`and for all Ns

            // If no match was found (should not happen), throw an exception
            [[unlikely]] if (!handled)
            {
                throw std::invalid_argument("Attempted to add zero cards to hand");
            }
        };
        // Generate the sequence [0, 1, ..., kMaxHandSize] at compile time and call dispatch with it
        dispatch(std::make_integer_sequence<size_t, kMaxHandSize + 1>{});
    }
    else
    {
        throw std::invalid_argument("Attempted to add zero cards to hand");
    }
}

template <typename FactionType>
void Faction<FactionType>::remove_cards_from_hand(const std::vector<uint8_t> &indices) {
    // Get the current hand size
    const uint8_t currentHandSize = get_hand_size();

    // Validate that all indices are within bounds
    [[unlikely]] if (std::any_of(indices.begin(), indices.end(),
        [currentHandSize](uint8_t index)
        { return index >= currentHandSize; }))
    {
        throw std::invalid_argument("Attempted to Set Invalid card");
    }

    // Get the current hand contents
    std::vector<::game_data::card_data::CardID> handContentsVector = get_hand_contents();

    // Create a mask to mark which indices should be removed
    std::vector<bool> removeMask(currentHandSize, false);
    for (uint8_t index : indices) {
        removeMask[index] = true;
    }

    // Build the new hand by skipping cards marked for removal
    std::vector<::game_data::card_data::CardID> newHand;
    newHand.reserve(currentHandSize - indices.size());
    for (uint8_t i = 0; i < currentHandSize; ++i) {
        if (!removeMask[i])
            newHand.push_back(handContentsVector[i]);
    }

    set_hand_contents(newHand);
}

template <typename FactionType>
[[nodiscard]] inline uint8_t Faction<FactionType>::get_remaining_pawn_count() const requires HasPawns<FactionType> {
    return read_bits<uint8_t, kPawnOffset, FactionType::kPawnBits>();
}

template <typename FactionType>
inline void Faction<FactionType>::set_remaining_pawn_count(uint8_t newCount) requires HasPawns<FactionType> {
    write_bits<uint8_t, kPawnOffset, FactionType::kPawnBits>(newCount);
}

template <typename FactionType>
template <typename DeckType>
inline void Faction<FactionType>::draw_cards(::game_data::deck_data::Deck<DeckType> &deck, uint8_t count)
{
    std::vector<::game_data::card_data::CardID> newCards = deck.get_cards_in_deck(0, count - 1);
    add_cards_to_hand(newCards);

}
} // faction_data
} // game_data