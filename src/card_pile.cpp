#include "../include/card_pile.hpp"

namespace game_data {
namespace pile_data {

[[nodiscard]] inline std::expected<uint8_t, PileError> CardPile::get_pile_size() const
{
    static_assert(kPileSizeBits > 0 && kPileSizeBits < 9, "Invalid kPileSizeBits value");
    const std::expected<uint8_t, PileError> size = read_bits<uint8_t, kPileSizeOffset, kPileSizeBits>();
    [[unlikely]] if (size.value() > card_data::kTotalCards)
        return std::unexpected(PileError{PileError::Code::kPileSizeExceededTotalItems});
    return size;
}

template <uint8_t newSize>
inline void CardPile::set_pile_size()
{
    static_assert(kPileSizeBits > 0 && kPileSizeBits < 9, "Invalid kPileSizeBits value");
    static_assert(newSize <= card_data::kTotalCards, "Card pile size cannot be greater than the quantity of cards");
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

    std::expected<std::vector<card_data::CardID>, PileError> result;
    result.value().reserve(pileSize.value());
    auto dispatch = [this, pileSize, &result]<size_t... Ns>(std::index_sequence<Ns...>) {
        (((pileSize.value() == (Ns + 2)) ?
            ([this, &result] {
                constexpr size_t N = Ns + 2;
                std::array<card_data::CardID, N> tempArray = read_bits<std::array<card_data::CardID, N>, kPileContentOffset, card_data::kCardIDBits>();
                result.value().assign(tempArray.begin(), tempArray.end());
            }(), void()) : void()), ...);
    };
    dispatch(std::make_index_sequence<card_data::kTotalCards - 1>{});
    return result;
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
    auto dispatch = [this, newPileSize, &newPile]<size_t... Ns>(std::index_sequence<Ns...>) -> std::expected<void, PileError> {
        bool handled = false;
        (((newPileSize == (Ns + 2)) ?
            ([this, &newPile, &handled] {
                constexpr size_t N = Ns + 2;
                std::array<card_data::CardID, N> tempArray{};
                std::copy_n(newPile.begin(), N, tempArray.begin());
                write_bits<std::array<card_data::CardID, N>, kPileContentOffset, card_data::kCardIDBits>(tempArray);
                set_pile_size<N>();
                handled = true;
            }(), void()) : void()), ...);
        [[unlikely]] if (handled == false)
            return std::unexpected(PileError{PileError::Code::kNewPileSizeExceededTotalItems});
        return {};
    };
    return dispatch(std::make_index_sequence<card_data::kTotalCards - 1>{});
}

[[nodiscard]] std::expected<std::vector<card_data::CardID>, PileError> CardPile::get_cards_in_pile(const std::vector<uint8_t> &desiredCardIndices) const
{
    static_assert(card_data::kCardIDBits > 0 && card_data::kCardIDBits <= 8, "Invalid kCardIDBits value");

    [[unlikely]] if (desiredCardIndices.empty())
        return std::unexpected(PileError{PileError::Code::kGetZeroItems});

    const auto pileSize = get_pile_size();
    [[unlikely]] if (!pileSize.has_value())
        return std::unexpected(pileSize.error());

    std::bitset<card_data::kTotalCards> seen;
    std::expected<std::vector<card_data::CardID>, PileError> result;
    result.value().reserve(desiredCardIndices.size());
    for (uint8_t index : desiredCardIndices) {
        [[unlikely]] if (index >= pileSize.value())
            return std::unexpected(PileError{PileError::Code::kIndexExceededCurrentPileSize});

        [[unlikely]] if (seen.test(index))
            return std::unexpected (PileError{PileError::Code::kDuplicateIndices});

        seen.set(index);
        auto dispatch = [this, index, &result]<size_t... Ns>(std::index_sequence<Ns...>) {
            (((index == Ns) ?
                ([this, &result] {
                    result.value().push_back(read_bits<card_data::CardID, Ns * card_data::kCardIDBits + kPileContentOffset, card_data::kCardIDBits>());
                }(), void()) : void()), ...);
        };
        dispatch(std::make_index_sequence<card_data::kTotalCards>{});
    }
    return result;
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
    std::expected<std::vector<card_data::CardID>, PileError> result;
    auto dispatch = [this, totalIndices, startIndex, &result]<size_t... Ns>(std::index_sequence<Ns...>) {
        (((totalIndices == (Ns + 1)) ?
            ([this, startIndex, &result] {
                constexpr size_t N = Ns + 1;
                std::array<card_data::CardID, N> tempArray =
                    read_bits<std::array<card_data::CardID, N>, card_data::kCardIDBits>(kPileContentOffset + startIndex * card_data::kCardIDBits);
                result.value().assign(tempArray.begin(), tempArray.end());
            }(), void()) : void()), ...);
    };
    dispatch(std::make_index_sequence<card_data::kTotalCards>{});
    return result;
}

std::expected<void, PileError> CardPile::set_cards_in_pile(const std::vector<IndexCardPair> &newIndexCardPairs)
{
    static_assert(card_data::kCardIDBits > 0 && card_data::kCardIDBits <= 8, "Invalid kCardIDBits Value");

    [[unlikely]] if (newIndexCardPairs.empty())
        return std::unexpected(PileError{PileError::Code::kSetZeroItems});

    const auto pileSize = get_pile_size();
    [[unlikely]] if (!pileSize.has_value())
        return std::unexpected(pileSize.error());

    std::bitset<card_data::kTotalCards> seen;
    for (const auto& pair : newIndexCardPairs) {
        [[unlikely]] if (pair.index >= pileSize.value())
            return std::unexpected(PileError{PileError::Code::kIndexExceededCurrentPileSize});

        [[unlikely]] if (seen.test(pair.index))
            return std::unexpected(PileError{PileError::Code::kDuplicateIndices});

        seen.set(pair.index);
        write_bits<card_data::CardID, card_data::kCardIDBits>(
            pair.cardID, 
            pair.index * card_data::kCardIDBits + kPileContentOffset
        );
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


    std::optional<PileError::Code> optErrorCode;
    const uint16_t offset = card_data::kCardIDBits * oldPileSize.value() + kPileContentOffset;
    if (newCardsCount == 1)
    {
        const auto result = set_pile_size(oldPileSize.value() + 1);
        [[likely]] if (result.has_value())
            { write_bits<card_data::CardID, card_data::kCardIDBits>(newCards[0], offset); }
        else
             { optErrorCode = result.error().code; };
    } else {
        auto count_dispatch = [this, newCardsCount, &newCards, oldPileSize, &optErrorCode]<size_t... Ns>(std::index_sequence<Ns...>)
        {
            (((newCardsCount == (Ns + 2)) ? ([this, &newCards, oldPileSize, &optErrorCode] {
                constexpr size_t N = Ns + 1;
                const auto result = set_pile_size(oldPileSize.value() + N);
                [[likely]] if (result.has_value()) {
                    const uint16_t offset = card_data::kCardIDBits * oldPileSize.value() + kPileContentOffset;
                    std::array<card_data::CardID, N> tempArray{};
                    std::copy_n(newCards.begin(), N, tempArray.begin());
                    write_bits<std::array<card_data::CardID, N>, card_data::kCardIDBits>(tempArray, offset);
                } else { optErrorCode = result.error().code; }
            }(), void()) : void()), ...);
        };
        count_dispatch(std::make_index_sequence<card_data::kTotalCards - 1>{});
    }

    [[unlikely]] if (optErrorCode.has_value())
        return std::unexpected(PileError{optErrorCode.value()});

    return {};
}

std::expected<void, PileError> CardPile::remove_cards_from_pile(const std::vector<uint8_t> &indices)
{
    [[unlikely]] if (indices.empty())
        return std::unexpected(PileError{PileError::Code::kRemoveZeroItems});

    const auto oldPileContentsVector = get_pile_contents();
    [[unlikely]] if (!oldPileContentsVector.has_value())
        return std::unexpected(oldPileContentsVector.error());

    const uint8_t oldSize = oldPileContentsVector.value().size();
    [[unlikely]] if (indices.size() > oldSize)
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
