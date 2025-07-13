#include "../include/discard_pile_data.hpp"

namespace game_data
{
namespace discard_pile_data
{
// Copy pasted from factions_data, and modified for only uint8_t -- fix later (or not)
template<::game_data::IsUint8OrEnumUint8 OutputType, uint16_t shift, uint8_t width>
[[nodiscard]] OutputType DiscardPile::read_bits() const {
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= 8, "Width cannot exceed one byte");

    // THIS IS ONLY COMPILE TIME CHECKING PLEASE DON'T BE STUPID, FORGET, AND RUIN IT
    constexpr uint8_t lastByte = (shift + width - 1) / 8;
    static_assert(lastByte < sizeof(discardPileData), "Not enough data to read requested bits");

    constexpr uint8_t byteIndex = shift / 8;
    constexpr uint8_t bitOffset = shift % 8;

    OutputType value = OutputType(0);

    // Fast path: byte-aligned reads can use direct access when width is exactly one byte
    if constexpr (bitOffset == 0 && width == 8)
    {
        value = static_cast<OutputType>(discardPileData[byteIndex]);
    }
    else
    {
        uint8_t temp = discardPileData[byteIndex];

        // Handle case where we need bits from a second byte
        if constexpr (bitOffset + width > 8)
            temp |= discardPileData[byteIndex + 1] << (8 - bitOffset);

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
template<::game_data::IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
[[nodiscard]] OutputType DiscardPile::read_bits() const {
    static_assert(elementWidth > 0, "Element width cannot be zero");
    static_assert(elementWidth <= 8, "Element width cannot exceed one byte");

    // Calculate the index of the last byte we'll need to access
    constexpr uint8_t lastByte = (shift + (sizeof(OutputType) - 1) * elementWidth) / 8;
    static_assert(lastByte < sizeof(discardPileData), "Not enough data to read requested bits");

    OutputType value = {};

    //maybe do SIMD but this is probably good enough
    // Fast path: contiguous, byte-aligned data can use direct memcpy
    if constexpr (elementWidth == 8 && (shift % 8 == 0))
    {
        std::memcpy(&value, &discardPileData[shift / 8], sizeof(OutputType));
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
                    uint8_t byteValue = discardPileData[byteIndex];

                    // If the bits for this element cross a byte boundary,
                    // also read the next byte and combine
                    if constexpr (bitOffset + elementWidth > 8)
                    {
                        byteValue |= discardPileData[byteIndex + 1] << (8 - bitOffset);
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
template<::game_data::IsUint8OrEnumUint8 InputType, uint16_t shift, uint16_t width>
void DiscardPile::write_bits(const InputType &value) {
    static_assert(width > 0, "Width cannot be zero");
    static_assert(width <= 8, "Width cannot exceed one byte");

    // THIS IS ONLY COMPILE TIME CHECKING PLEASE DON'T BE STUPID, FORGET, AND RUIN IT
    constexpr uint8_t lastByte = (shift + width - 1) / 8;
    static_assert(lastByte < sizeof(discardPileData), "Not enough data to write requested bits");

    constexpr uint8_t byteIndex = shift / 8;
    constexpr uint8_t bitOffset = shift % 8;

    // Fast path: byte-aligned writes can use direct memcpy when width matches the type size exactly
    if constexpr (bitOffset == 0 && width == 8)
    {
        std::memcpy(&discardPileData[byteIndex], &value, sizeof(InputType));
    }
    else
    {
        uint8_t value_uint8 = static_cast<uint8_t>(value);
        constexpr uint8_t mask = (width < 8) ? ((1U << width) - 1) : 0xFF;
        value_uint8 &= mask;
        // Clear the target bits in the first byte
        discardPileData[byteIndex] &= ~(mask << bitOffset);
        // Set the bits from value
        discardPileData[byteIndex] |= (value_uint8 << bitOffset);

        // Handle case where we need bits from a second byte
        if constexpr (bitOffset + width > 8) {
            constexpr uint8_t nextMask = (mask >> (8 - bitOffset));
            discardPileData[byteIndex + 1] &= ~nextMask;
            discardPileData[byteIndex + 1] |= (value_uint8 >> (8 - bitOffset)) & nextMask;
        }
    }
}

template<::game_data::IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
void DiscardPile::write_bits(const InputType &value) {
    static_assert(elementWidth > 0, "Element width cannot be zero");
    static_assert(elementWidth <= 8, "Element width cannot exceed one byte");

    // Calculate the index of the last byte we'll need to access
    constexpr uint8_t lastByte = (shift + (sizeof(InputType) - 1) * elementWidth) / 8;
    static_assert(lastByte < sizeof(discardPileData), "Not enough data to write requested bits");

    //maybe do SIMD but this is probably good enough
    // Fast path: contiguous, byte-aligned data can use direct memcpy
    if constexpr (elementWidth == 8 && (shift % 8 == 0))
    {
        std::memcpy(&discardPileData[shift / 8], &value, sizeof(InputType));
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
                discardPileData[byteIndex] &= ~(mask << bitOffset);
                // Set the bits from value
                discardPileData[byteIndex] |= (byteValue << bitOffset);

                // Handle case where we need bits from a second byte
                if constexpr (bitOffset + elementWidth > 8) {
                    constexpr uint8_t nextMask = (mask >> (8 - bitOffset));
                    discardPileData[byteIndex + 1] &= ~nextMask;
                    discardPileData[byteIndex + 1] |= (byteValue >> (8 - bitOffset)) & nextMask;
                }                      
            }()),
            // Fold expression repeats this for all indices
            ...);
        };

        // Execute the unrolled loop using compile-time index sequence
        unroll(std::make_index_sequence<sizeof(InputType)>{});
    }
}

[[nodiscard]] inline uint8_t DiscardPile::get_discard_pile_size() const {
    static_assert(kDiscardPileSizeBits > 0 && kDiscardPileSizeBits < 9, "Invalid kDiscardPileSizeBits value");

    return read_bits<uint8_t, kDiscardPileSizeBits, kDiscardPileSizeBits>();
}

template <uint8_t newSize>
inline void DiscardPile::set_discard_pile_size() {
    static_assert(kDiscardPileSizeBits > 0 && kDiscardPileSizeBits < 9, "Invalid kDiscardPileSizeBits value");
    static_assert(newSize <= ::game_data::card_data::kTotalCards, "Discard pile size cannot be greater than the quantity of cards");

    write_bits<uint8_t, kDiscardPileSizeOffset, kDiscardPileSizeBits>(newSize);
}

inline void DiscardPile::set_discard_pile_size(uint8_t newSize) {
    [[unlikely]] if (newSize > ::game_data::card_data::kTotalCards) {
        throw std::invalid_argument("New discard pile size cannot cannot be greater than the quantity of cards");
    }

    write_bits<uint8_t, kDiscardPileSizeOffset, kDiscardPileSizeBits>(newSize);
}

[[nodiscard]] std::vector<::game_data::card_data::CardID> DiscardPile::get_discard_pile_contents() const {
    const uint8_t discardPileSize = get_discard_pile_size();
    [[unlikely]] if (discardPileSize == 0) {
        return {};
    } else [[unlikely]] if (discardPileSize == 1) {
        // Fast path for a single card
        return {read_bits<::game_data::card_data::CardID, kDiscardPileContentOffset, ::game_data::card_data::kCardIDBits>()};
    } else if (discardPileSize <= ::game_data::card_data::kTotalCards) {
        std::vector<::game_data::card_data::CardID> result;
        // Compile-time dispatch for all possible discard pile sizes (starting from 2)
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            bool handled = false;
            (((discardPileSize == (Ns + 2)) ?
                ([&] {
                    constexpr size_t N = Ns + 2;
                    std::array<::game_data::card_data::CardID, N> tempArray =
                        read_bits<std::array<::game_data::card_data::CardID, N>, kDiscardPileContentOffset, ::game_data::card_data::kCardIDBits>();
                    result.assign(tempArray.begin(), tempArray.end());
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("Discard pile size exceeds maximum");
            }
        };
        dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards - 1>{});
        return result;
    } else {
        throw std::invalid_argument("Discard pile size exceeds maximum");
    }
}

void DiscardPile::set_discard_pile_contents(const std::vector<::game_data::card_data::CardID> &newDiscardPile) {
    // Helper lambda to write N cards to discard pile and set the discard pile size at compile time
    auto set_discard_pile_universal = [&](auto N) {
        constexpr size_t kN = decltype(N)::value;
        std::array<::game_data::card_data::CardID, kN> tempArray{};
        std::copy_n(newDiscardPile.begin(), kN, tempArray.begin());
        write_bits<std::array<::game_data::card_data::CardID, kN>, kDiscardPileContentOffset, ::game_data::card_data::kCardIDBits>(tempArray);
        set_discard_pile_size<kN>();
    };

    uint8_t newDiscardPileSize = newDiscardPile.size();

    [[unlikely]] if (newDiscardPileSize == 0) {
        set_discard_pile_size<0>();
    } else [[unlikely]] if (newDiscardPileSize == 1) {
        // Fast path for a single card
        write_bits<::game_data::card_data::CardID, kDiscardPileContentOffset, ::game_data::card_data::kCardIDBits>(newDiscardPile[0]);
        set_discard_pile_size<1>();
    } else [[likely]] if (newDiscardPileSize <= ::game_data::card_data::kTotalCards) {
        // Compile-time dispatch for all possible discard pile sizes (starting from 2)
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            bool handled = false;
            (((newDiscardPileSize == (Ns + 2)) ?
                ([&] {
                    set_discard_pile_universal(std::integral_constant<size_t, Ns + 2>{});
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("DiscardPile size exceeds maximum");
            }
        };
        dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards - 1>{});
    } else {
        throw std::invalid_argument("DiscardPile size exceeds maximum");
    }
}

[[nodiscard]] std::vector<::game_data::card_data::CardID> DiscardPile::get_cards_in_discard_pile(const std::vector<uint8_t> &desiredCardIndices) const {
    [[unlikely]] if (desiredCardIndices.empty())
        throw std::invalid_argument("Cannot get 0 cards");

    std::vector<uint8_t> sortedIndices = desiredCardIndices;
    std::sort(sortedIndices.begin(), sortedIndices.end());

    [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end()) {
        throw std::invalid_argument("Duplicate indices are not allowed");
    }

    // Get the current discard pile size
    const uint8_t currentDiscardPileSize = get_discard_pile_size();

    [[unlikely]] if (sortedIndices.back() >= currentDiscardPileSize) {
        throw std::invalid_argument("Attempted to set invalid card");
    }

    std::vector<::game_data::card_data::CardID> result;
    result.reserve(desiredCardIndices.size());
    for (uint8_t idx : desiredCardIndices) {
        // Compile-time dispatch for each possible index
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
            bool handled = false;
            (((idx == Ns) ?
                ([&] {
                    result.push_back(read_bits<::game_data::card_data::CardID, Ns * ::game_data::card_data::kCardIDBits + kDiscardPileContentOffset, ::game_data::card_data::kCardIDBits>());
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

[[nodiscard]] std::vector<::game_data::card_data::CardID> DiscardPile::get_cards_in_discard_pile(uint8_t startIndex, uint8_t endIndex) const {
    const uint8_t discardPileSize = get_discard_pile_size();

    [[unlikely]] if (startIndex >= discardPileSize || endIndex >= discardPileSize)
        throw std::invalid_argument("Index exceeded maximum card index");

    [[unlikely]] if (startIndex >= endIndex)
        throw std::invalid_argument("Start index >= end index");

    const uint8_t totalIndices = endIndex - startIndex;
    
    // probably making a compile-time version of read_bits would be faster than a dispatch but i don't care at this point. if you're seeing this its fine when I profile it later
    std::vector<::game_data::card_data::CardID> result;
    result.reserve(totalIndices);
    auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
        ((Ns >= startIndex && Ns < endIndex ?
            result.push_back(read_bits<::game_data::card_data::CardID, Ns * ::game_data::card_data::kCardIDBits + kDiscardPileContentOffset, ::game_data::card_data::kCardIDBits>())
            : void()), ...);
    };
    dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards>{});
    return result;
}

void DiscardPile::set_cards_in_discard_pile(const std::vector<std::pair<uint8_t, ::game_data::card_data::CardID>> &newIndexCardPairs) {
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
                    write_bits<::game_data::card_data::CardID, Ns * ::game_data::card_data::kCardIDBits + kDiscardPileContentOffset, ::game_data::card_data::kCardIDBits>(pair.second);
                    handled = true;
                }(), void()) : void()), ...);
            if (!handled) {
                throw std::invalid_argument("Desired card index exceeded maximum card index (dispatch)");
            }
        };
        dispatch(std::make_index_sequence<::game_data::card_data::kTotalCards>{});
    }

    set_discard_pile_size(highestIndex + 1);
}

void DiscardPile::add_cards_to_discard_pile(const std::vector<::game_data::card_data::CardID> &newCards) {
    const uint8_t newCardsCount = newCards.size();
    const uint8_t oldDiscardPileSize = get_discard_pile_size();

    [[unlikely]] if (newCardsCount == 0)
        throw std::invalid_argument("Attempted to add zero cards to discard pile");

    [[unlikely]] if (newCardsCount + oldDiscardPileSize > ::game_data::card_data::kTotalCards)
        throw std::invalid_argument("Attempted to add cards above the discard pile size limit");

    [[unlikely]] if (std::any_of(newCards.begin(), newCards.end(),
        [](::game_data::card_data::CardID card)
        { return card == ::game_data::card_data::CardID::kNotACard; }))
    {
        throw std::invalid_argument("Attempted to Set Invalid card");
    }

    // Helper lambda to write N cards to discard pile and set the discard pile size at runtime
    auto add_cards_universal_runtime = [&](size_t N, uint16_t offset) {
        std::array<::game_data::card_data::CardID, N> tempArray(newCards.begin(), newCards.begin() + N);
        write_bits<std::array<::game_data::card_data::CardID, N>, offset, ::game_data::card_data::kCardIDBits>(tempArray);
        set_discard_pile_size(oldDiscardPileSize + N);
    };

    if (newCardsCount == 1)
    {
        uint16_t offset = ::game_data::card_data::kCardIDBits * oldDiscardPileSize + kDiscardPileContentOffset;
        write_bits<::game_data::card_data::CardID, offset, ::game_data::card_data::kCardIDBits>(newCards[0]);
        set_discard_pile_size(oldDiscardPileSize + 1);
    }
    else [[likely]] if (newCardsCount <= ::game_data::card_data::kTotalCards)
    {
        add_cards_universal_runtime(newCardsCount, ::game_data::card_data::kCardIDBits * oldDiscardPileSize + kDiscardPileContentOffset);
    }
    else
    {
        throw std::invalid_argument("Attempted to add an invalid number of cards to discard pile");
    }
}

void DiscardPile::remove_cards_from_discard_pile(const std::vector<uint8_t> &indices) {

    [[unlikely]] if (indices.empty())
        throw std::invalid_argument("Cannot remove 0 cards");

    std::vector<uint8_t> sortedIndices = indices;

    std::sort(sortedIndices.begin(), sortedIndices.end());
    [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end()) {
        throw std::invalid_argument("Duplicate indices are not allowed");
    }

    // Get the current discard pile size
    const uint8_t currentDiscardPileSize = get_discard_pile_size();

    [[unlikely]] if (sortedIndices.back() >= currentDiscardPileSize) {
        throw std::invalid_argument("Attempted to set invalid card");
    }

    // Get the current discard pile contents
    std::vector<::game_data::card_data::CardID> discardPileContentsVector = get_discard_pile_contents();

    // Build the new discard pile by skipping sorted indices in a single pass
    std::vector<::game_data::card_data::CardID> newDiscardPile;
    newDiscardPile.reserve(currentDiscardPileSize - sortedIndices.size());
    size_t removeIdx = 0;
    for (uint8_t i = 0; i < currentDiscardPileSize; ++i) {
        // If current i matches the next index to remove, skip it
        if (removeIdx < sortedIndices.size() && i == sortedIndices[removeIdx]) {
            ++removeIdx;
            continue;
        }
        newDiscardPile.push_back(discardPileContentsVector[i]);
    }

    set_discard_pile_contents(newDiscardPile);
}


} // discard_pile_data
} // game_data