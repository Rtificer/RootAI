#pragma once

#include "card_data.hpp"
#include "game_data.hpp"

#include <array>
#include <cstdint>
#include <vector>
#include <expected>
#include <bitset>
#include <optional>
#include <algorithm>

namespace game_data {
namespace pile_data {

namespace card_data = ::game_data::card_data;
namespace game_data = ::game_data;

struct PileError {
    const enum class Code : uint8_t {
        kPileSizeExceededTotalItems,
        kNewPileSizeExceededTotalItems,
        kGetZeroItems,
        kSetZeroItems,
        kAddZeroItems,
        kRemoveZeroItems,
        kDuplicateIndices,
        kIndexExceededCurrentPileSize,
        kStartIndexMustNotExceedEndIndex,
        kPileSizeUnderflow,
        kInvalidOperation
    } code;

    static constexpr std::array<std::string_view, 12> kMessages = {
        "Pile size exceeded total items",
        "New pile size exceeded total items",
        "Cannot get 0 items from pile",
        "Cannot set 0 items in pile",
        "Cannot add 0 items to pile",
        "Cannot remove 0 items from pile",
        "Duplicate indices are disallowed",
        "Index exceeded maximum pile size",
        "Start index of range must not exceed end index",
        "Cannot remove more items than remain in pile",
        "Invalid operation error. This is not meant for human eyes, congratulations you nuked the program. Check add_cards_to_pile()",
        "Unknown error"
    };

    [[nodiscard]] static std::string_view to_string(Code code) {
        uint8_t idx = static_cast<uint8_t>(code);
        [[likely]] if (idx < kMessages.size()) return kMessages[idx];
        return kMessages.back(); // Unknown Error
    }

    [[nodiscard]] inline std::string_view message() const { return to_string(code); }
};

struct IndexCardPair {
    uint8_t index;
    card_data::CardID cardID;
};

// Abstract CardPile class for handling piles of CardID
class CardPile {
public:
    virtual ~CardPile() = default;

    [[nodiscard]] inline std::expected<uint8_t, PileError> get_pile_size() const;
    template <uint8_t newSize>
    inline void set_pile_size();
    [[nodiscard]] inline std::expected<void, PileError> set_pile_size(uint8_t newSize);

    [[nodiscard]] std::expected<std::vector<card_data::CardID>, PileError> get_pile_contents() const;
    [[nodiscard]] std::expected<void, PileError> set_pile_contents(const std::vector<card_data::CardID> &newPile);
    [[nodiscard]] std::expected<std::vector<card_data::CardID>, PileError> get_cards_in_pile(const std::vector<uint8_t> &desiredCardIndices) const;
    [[nodiscard]] std::expected<std::vector<card_data::CardID>, PileError> get_cards_in_pile(uint8_t startIndex, uint8_t endIndex) const;
    [[nodiscard]] std::expected<void, PileError> set_cards_in_pile(const std::vector<IndexCardPair> &newIndexCardPairs);
    [[nodiscard]] std::expected<void, PileError> add_cards_to_pile(const std::vector<card_data::CardID> &newCards);
    [[nodiscard]] std::expected<void, PileError> remove_cards_from_pile(const std::vector<uint8_t> &indices);
    [[nodiscard]] inline std::expected<void, PileError> pop_cards_from_pile(uint8_t popCardCount);

protected:
    //Enforce abstractness
    CardPile() = default;
    
    static constexpr uint8_t kPileSizeBits = 6;
    static constexpr uint16_t kPileContentBits = card_data::kTotalCards * card_data::kCardIDBits;

    static constexpr uint16_t kPileSizeOffset = 0;
    static constexpr uint16_t kPileContentOffset = kPileSizeOffset + kPileSizeBits;

    using CardPileData = std::array<uint8_t, (kPileSizeBits + card_data::kCardIDBits * card_data::kTotalCards + 7) / 8>;

    CardPileData pileData;
    
    inline void on_pile_empty() {}; // CRTP

    virtual consteval CardPileData initialize_pile() const = 0;

    // Wrappers for read and write bits functions to allow for ease of use
    template <game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t shift, uint16_t width>
    [[nodiscard]] inline OutputType read_bits() const {
        return game_data::read_bits<OutputType, sizeof(pileData), shift, width>(pileData);
    }

    template <game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t width>
    [[nodiscard]] inline std::expected<OutputType, game_data::ReadWriteError> read_bits(uint16_t shift) const {
        return game_data::read_bits<OutputType, sizeof(pileData), width>(pileData, shift);
    }

    template <game_data::IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
    [[nodiscard]] inline OutputType read_bits() const {
        return game_data::read_bits<OutputType, sizeof(pileData), shift, elementWidth>(pileData);
    }

    template <game_data::IsValidByteVector OutputType, uint16_t shift, uint8_t elementWidth>
    [[nodiscard]] inline std::expected<OutputType, game_data::ReadWriteError> read_bits(uint16_t outputSize) const {
        return game_data::read_bits<OutputType, sizeof(pileData), shift, elementWidth>(pileData, outputSize);
    }

    template <game_data::IsValidByteArray OutputType, uint8_t elementWidth>
    [[nodiscard]] inline std::expected<OutputType, game_data::ReadWriteError> read_bits(uint16_t shift) const {
        return game_data::read_bits<OutputType, sizeof(pileData), elementWidth>(pileData, shift);
    }

    template <game_data::IsValidByteVector OutputType, uint8_t elementWidth>
    [[nodiscard]] inline std::expected<OutputType, game_data::ReadWriteError> read_bits(uint16_t outputSize, uint16_t shift) const {
        return game_data::read_bits<OutputType, sizeof(pileData), elementWidth>(pileData, outputSize, shift);
    }

    template <game_data::IsUnsignedIntegralOrEnum InputType, uint16_t shift, uint16_t width>
    inline void write_bits(const InputType &value) {
        game_data::write_bits<InputType, sizeof(pileData), shift, width>(pileData, value);
    }

    template <game_data::IsUnsignedIntegralOrEnum InputType, uint16_t width>
    [[nodiscard]] inline std::expected<void, game_data::ReadWriteError> write_bits(const InputType &value, uint16_t shift) {
        return game_data::write_bits<InputType, sizeof(pileData), width>(pileData, value, shift);
    }

    template <game_data::IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
    inline void write_bits(const InputType &value) {
        game_data::write_bits<InputType, sizeof(pileData), shift, elementWidth>(pileData, value);
    }

    template <game_data::IsValidByteVector InputType, uint16_t shift, uint8_t elementWidth>
    [[nodiscard]] inline std::expected<void, game_data::ReadWriteError> write_bits(uint16_t outputSize, const InputType &value) {
       return game_data::write_bits<InputType, sizeof(pileData), shift, elementWidth>(pileData, outputSize, value);
    }

    template <game_data::IsValidByteArray InputType, uint8_t elementWidth>
    [[nodiscard]] inline std::expected<void, game_data::ReadWriteError> write_bits(const InputType &value, uint16_t shift) {
        return game_data::write_bits<InputType, sizeof(pileData), elementWidth>(pileData, value, shift);
    }

    template <game_data::IsValidByteVector InputType, uint8_t elementWidth>
    [[nodiscard]] inline std::expected<void, game_data::ReadWriteError> write_bits(uint16_t outputSize, const InputType &value, uint16_t shift) {
        return game_data::write_bits<InputType, sizeof(pileData), elementWidth>(pileData, outputSize, value, shift);
    }
};
} // namespace pile_data
} // namespace game_data
