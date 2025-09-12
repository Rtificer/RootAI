#include "../include/card_pile.hpp"

namespace game_data {
namespace pile_data {

[[nodiscard]] inline std::expected<uint8_t, PileError> CardPile::get_pile_size() const
{
    static_assert(kPileSizeBits > 0 && kPileSizeBits < 9, "Invalid kPileSizeBits value");

    const uint8_t size = read_bits<uint8_t, kPileSizeOffset, kPileSizeBits>();

    [[unlikely]] if (size > card_data::kTotalCards)
        return std::unexpected(PileError{PileError::Code::kPileSizeExceededTotalItems});

    return std::expected<uint8_t, PileError>{size};
}

template <uint8_t newSize>
inline void CardPile::set_pile_size()
{
    static_assert(kPileSizeBits > 0 && kPileSizeBits < 9, "Invalid kPileSizeBits value");
    static_assert(newSize <= card_data::kTotalCards, "Card pile size cannot be greater than the quantity of cards");

    if constexpr (newSize == 0)
        this->on_pile_empty();

    write_bits<uint8_t, kPileSizeOffset, kPileSizeBits>(newSize);
}

inline std::expected<void, PileError> CardPile::set_pile_size(uint8_t newSize)
{
    static_assert(kPileSizeBits > 0 && kPileSizeBits < 9, "Invalid kPileSizeBits value");
    [[unlikely]] if (newSize > card_data::kTotalCards)
        return std::unexpected(PileError{PileError::Code::kNewPileSizeExceededTotalItems});
        
    write_bits<uint8_t, kPileSizeOffset, kPileSizeBits>(newSize);
    return {};
}

[[nodiscard]] std::expected<std::vector<card_data::CardID>, PileError> CardPile::get_pile_contents() const
{
    const auto pileSize = get_pile_size();
    [[unlikely]] if (!pileSize.has_value())
        return std::unexpected(pileSize.error());


    [[unlikely]] if (pileSize.value() == 0)
        return std::expected<std::vector<card_data::CardID>, PileError>(std::vector<card_data::CardID>{});

    [[unlikely]] if (pileSize.value() == 1)
        return std::expected<std::vector<card_data::CardID>, PileError>(
            std::vector<card_data::CardID>{read_bits<card_data::CardID, kPileContentOffset, card_data::kCardIDBits>()});

    return read_bits<std::vector<card_data::CardID>, kPileContentOffset, card_data::kCardIDBits>(pileSize.value())
        .transform_error([](game_data::ReadWriteError error)
            { return PileError{static_cast<PileError::Code>(error.code)}; });
}

std::expected<void, PileError> CardPile::set_pile_contents(const std::vector<card_data::CardID> &newPile)
{
    const uint8_t newPileSize = newPile.size();

    [[unlikely]] if (newPileSize == 0) {
        set_pile_size<0>();
        return {};
    }

    [[unlikely]] if (newPileSize == 1) {
        write_bits<card_data::CardID, kPileContentOffset, card_data::kCardIDBits>(newPile[0]);
        set_pile_size<1>();
        return {};
    }

    const std::expected<void, PileError> result = set_pile_size(newPileSize);
    [[unlikely]] if (!result.has_value())
        return result;

    return write_bits<std::vector<card_data::CardID>, kPileContentOffset, card_data::kCardIDBits>(newPileSize, newPile)
        .transform_error([](game_data::ReadWriteError error)
            { return PileError{static_cast<PileError::Code>(error.code)}; });
}

[[nodiscard]] std::expected<std::vector<card_data::CardID>, PileError> CardPile::get_cards_in_pile(const std::vector<uint8_t> &desiredCardIndices) const
{
    static_assert(card_data::kCardIDBits > 0 && card_data::kCardIDBits <= 8, "Invalid kCardIDBits value");

    [[unlikely]] if (desiredCardIndices.empty())
        return std::unexpected(PileError{PileError::Code::kGetZeroItems});

    const std::expected<std::vector<card_data::CardID>, PileError> pileContents = get_pile_contents();
    [[unlikely]] if (!pileContents.has_value())
        return pileContents;

    const uint8_t pileSize = pileContents.value().size();

    std::bitset<card_data::kTotalCards> seen;
    std::vector<card_data::CardID> result;
    result.reserve(desiredCardIndices.size());
    for (const uint8_t& index : desiredCardIndices) {
        [[unlikely]] if (index >= pileSize)
            return std::unexpected(PileError{PileError::Code::kIndexExceededCurrentPileSize});

        [[unlikely]] if (seen.test(index))
            return std::unexpected(PileError{PileError::Code::kDuplicateIndices});

        seen.set(index);

        result.push_back(pileContents.value()[index]);
    }

    return std::expected<std::vector<card_data::CardID>, PileError>{result};
}

[[nodiscard]] std::expected<std::vector<card_data::CardID>, PileError> CardPile::get_cards_in_pile(uint8_t startIndex, uint8_t endIndex) const
{
    const auto pileSize = get_pile_size();
    [[unlikely]] if (!pileSize.has_value())
        return std::unexpected(pileSize.error());

    [[unlikely]] if (startIndex >=pileSize.value() || endIndex >= pileSize.value())
        return std::unexpected(PileError{PileError::Code::kIndexExceededCurrentPileSize});

    [[unlikely]] if (startIndex >= endIndex)
        return std::unexpected(PileError{PileError::Code::kStartIndexMustNotExceedEndIndex});
        
    const uint8_t totalIndices = endIndex - startIndex;
    return read_bits<std::vector<card_data::CardID>, card_data::kCardIDBits>(totalIndices, kPileContentOffset + startIndex * card_data::kCardIDBits)
        .transform_error([](game_data::ReadWriteError error)
            { return PileError{static_cast<PileError::Code>(error.code)}; });
}

std::expected<void, PileError> CardPile::set_cards_in_pile(const std::vector<IndexCardPair> &newIndexCardPairs)
{
    static_assert(card_data::kCardIDBits > 0 && card_data::kCardIDBits <= 8, "Invalid kCardIDBits Value");

    const uint8_t totalNewCards = newIndexCardPairs.size();

    [[unlikely]] if (totalNewCards == 0)
        return std::unexpected(PileError{PileError::Code::kSetZeroItems});


    std::bitset<card_data::kTotalCards> seen;
    static constexpr uint8_t kBuildEditThreshold = 5;
    if (totalNewCards >= kBuildEditThreshold) {
        auto pileContentsResult = get_pile_contents();
        [[unlikely]] if (!pileContentsResult.has_value())
            return std::unexpected(pileContentsResult.error());

        auto& pileContents = pileContentsResult.value();
        const auto pileSize = pileContents.size();

        std::bitset<card_data::kTotalCards> seen;
        for (const auto& pair : newIndexCardPairs) {
            [[unlikely]] if (pair.index >= pileSize)
                return std::unexpected(PileError{PileError::Code::kIndexExceededCurrentPileSize});
            [[unlikely]] if (seen.test(pair.index))
                return std::unexpected(PileError{PileError::Code::kDuplicateIndices});
            seen.set(pair.index);
            pileContents[pair.index] = pair.cardID;
        }

        return set_pile_contents(pileContents);
    }

    for (const auto& pair : newIndexCardPairs) {
        const auto pileSize = get_pile_size();
        [[unlikely]] if (!pileSize.has_value())
            return std::unexpected(pileSize.error());

        [[unlikely]] if (pair.index >= pileSize.value())
            return std::unexpected(PileError{PileError::Code::kIndexExceededCurrentPileSize});

        [[unlikely]] if (seen.test(pair.index))
            return std::unexpected(PileError{PileError::Code::kDuplicateIndices});

        seen.set(pair.index);
        
        const std::expected<void, PileError> result = write_bits<card_data::CardID, card_data::kCardIDBits>(pair.cardID, pair.index * card_data::kCardIDBits + kPileContentOffset)
            .transform_error([](game_data::ReadWriteError error)
                { return PileError{static_cast<PileError::Code>(error.code)}; });

        [[unlikely]] if (!result.has_value())
            return result;
    }
    return {};
}

std::expected<void, PileError> CardPile::add_cards_to_pile(const std::vector<card_data::CardID> &newCards)
{
    const uint8_t newCardsCount = newCards.size();
    [[unlikely]] if (newCardsCount == 0)
        return std::unexpected(PileError{PileError::Code::kAddZeroItems});

    const auto oldPileSize = get_pile_size();
    [[unlikely]] if (!oldPileSize.has_value())
        return std::unexpected(oldPileSize.error());

    std::expected<void, PileError> setPileSizeResult = set_pile_size(oldPileSize.value() + newCardsCount);
    [[unlikely]] if (!setPileSizeResult.has_value())
        return setPileSizeResult;

    const uint16_t offset = card_data::kCardIDBits * oldPileSize.value() + kPileContentOffset;
    if (newCardsCount == 1)
        return write_bits<card_data::CardID, card_data::kCardIDBits>(newCards[0], offset)
            .transform_error([](game_data::ReadWriteError error)
                { return PileError{static_cast<PileError::Code>(error.code)}; });

    return write_bits<std::vector<card_data::CardID>, card_data::kCardIDBits>(newCardsCount, newCards, offset)
        .transform_error([](game_data::ReadWriteError error)
            { return PileError{static_cast<PileError::Code>(error.code)}; });
}

std::expected<void, PileError> CardPile::remove_cards_from_pile(const std::vector<uint8_t> &indices)
{
    const uint8_t removalCount = indices.size();
    [[unlikely]] if (removalCount == 0)
        return std::unexpected(PileError{PileError::Code::kRemoveZeroItems});

    const auto oldPileContentsVector = get_pile_contents();
    [[unlikely]] if (!oldPileContentsVector.has_value())
        return std::unexpected(oldPileContentsVector.error());

    const uint8_t oldSize = oldPileContentsVector.value().size();
    [[unlikely]] if (removalCount > oldSize)
        return std::unexpected(PileError{PileError::Code::kPileSizeUnderflow});

    std::vector<uint8_t> sortedIndices = indices;
    std::sort(sortedIndices.begin(), sortedIndices.end());
    [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end())
        return std::unexpected(PileError{PileError::Code::kDuplicateIndices});

    [[unlikely]] if (sortedIndices.back() >= oldSize)
        return std::unexpected(PileError{PileError::Code::kIndexExceededCurrentPileSize});
        
    std::vector<card_data::CardID> newPile;
    newPile.reserve(oldSize - sortedIndices.size());
    size_t removeIndex = 0;
    for (uint8_t i = 0; i < oldSize; ++i) {
        if (removeIndex < sortedIndices.size() && i == sortedIndices[removeIndex]) {
            ++removeIndex;
            continue;
        }
        newPile.push_back(oldPileContentsVector.value()[i]);
    }
    return set_pile_contents(newPile);
}

inline std::expected<void, PileError> CardPile::pop_cards_from_pile(uint8_t count)
{
    auto oldSizeResult = get_pile_size();
    [[unlikely]] if (!oldSizeResult.has_value())
        return std::unexpected(oldSizeResult.error());

    [[unlikely]] if (oldSizeResult.value() < count)
        return std::unexpected(PileError{PileError::Code::kPileSizeUnderflow});
        
    return set_pile_size(oldSizeResult.value() - count);
}

} // namespace pile_data
} // namespace game_data
