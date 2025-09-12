#include "../include/clearing_data.hpp"

namespace game_data
{
namespace board_data
{
namespace clearing_data
{

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline uint8_t Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::get_occupied_slot_count_unsafe() const
{
    static_assert(kBuildingSlotCountBits > 0 && kBuildingSlotCountBits <= 8, "Invalid kBuildingSlotCountBits value");
    return read_bits<uint8_t, kOccupiedBuildingSlotCountOffset, kOccupiedBuildingSlotCountBits>();
}   

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline std::expected<uint8_t, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::get_slot_count() const 
{
    static_assert(kBuildingSlotCountBits > 0 && kBuildingSlotCountBits <= 8, "Invalid kBuildingSlotCountBits value");

    const uint8_t count = read_bits<uint8_t, kBuildingSlotCountOffset, kBuildingSlotCountBits>();

    [[unlikely]] if (count > kMaxBuildingSlotCount)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kSlotCountExceededMaximumSlotCount});

    [[unlikely]] if (count < get_occupied_slot_count_unsafe())
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kSlotCountWasLessThanOccupiedSlotCount});
    
    return std::expected<uint8_t, building_data::BuildingError>{count};
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline std::expected<uint8_t, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::get_occupied_slot_count() const
{
    static_assert(kOccupiedBuildingSlotCountBits > 0 && kOccupiedBuildingSlotCountBits <= 8, "Invalid kOccupiedBuildingSlotCountBits value");


    const uint8_t occupiedSlots = read_bits<uint8_t, kOccupiedBuildingSlotCountOffset, kOccupiedBuildingSlotCountBits>();
    return get_slot_count().and_then([occupiedSlots](uint8_t totalSlots) -> std::expected<uint8_t, building_data::BuildingError> {
        [[unlikely]] if (occupiedSlots > totalSlots)
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kOccupiedExceededCurrentSlotCount});
        return occupiedSlots;
    }).or_else([](building_data::BuildingError error) -> 
        std::expected<uint8_t, building_data::BuildingError> 
            { return std::unexpected<building_data::BuildingError>(error); });
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline std::expected<uint8_t, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::get_remaining_slot_count() const
{
    static_assert(kBuildingSlotCountBits + kOccupiedBuildingSlotCountBits > 0 && kBuildingSlotCountBits + kOccupiedBuildingSlotCountBits <= 8, "Invalid combined kBuildingSlotCountBits and kOccupiedBuildingSlotCountOffset value");

    uint8_t combined = read_bits<uint8_t, kBuildingSlotCountOffset, kBuildingSlotCountBits + kOccupiedBuildingSlotCountBits>();

    const uint8_t remainingSlots = (combined & ((1 << kBuildingSlotCountBits) - 1)) - (combined >> kBuildingSlotCountBits);
    [[unlikely]] if (remainingSlots > kMaxBuildingSlotCount)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewOccupiedCountExceededMaximumSlotCount});
    
    return std::expected<uint8_t, building_data::BuildingError>{remainingSlots};
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
template<uint8_t newCount>
inline std::expected<void, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_slot_count()
{
    static_assert(kBuildingSlotCountBits > 0 && kBuildingSlotCountBits <= 8, "Invalid kBuildingSlotCountBits value");
    static_assert(newCount <= kMaxBuildingSlotCount, "newCount must not exceed max building slot count");

    return get_occupied_slot_count().and_then([this](uint8_t oldCount) -> std::expected<void, building_data::BuildingError> {
        [[unlikely]] if (newCount < oldCount)
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewSlotCountWasLessThanOccupiedSlotCount});

        write_bits<uint8_t, kBuildingSlotCountOffset, kBuildingSlotCountBits>(newCount); 
        return {};
    }).or_else([](building_data::BuildingError error) -> 
        std::expected<void, building_data::BuildingError> 
            { return std::unexpected<building_data::BuildingError>(error); });
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
inline std::expected<void, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_slot_count(uint8_t newCount)
{
    static_assert(kBuildingSlotCountBits > 0 && kBuildingSlotCountBits <= 8, "Invalid kBuildingSlotCountBits value");
    [[unlikely]] if (newCount > kMaxBuildingSlotCount)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewSlotCountExceededMaximumSlotCount});

    return get_occupied_slot_count().and_then([this, newCount](uint8_t oldCount) -> std::expected<void, building_data::BuildingError> {
        [[unlikely]] if (newCount < oldCount)
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewSlotCountWasLessThanOccupiedSlotCount});

        write_bits<uint8_t, kBuildingSlotCountOffset, kBuildingSlotCountBits>(newCount); 
        return {};
    }).or_else([](building_data::BuildingError error) -> 
    std::expected<void, building_data::BuildingError> { return std::unexpected<building_data::BuildingError>(error); });
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
inline std::expected<void, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_occupied_slot_count(uint8_t newCount)
{
    static_assert(kOccupiedBuildingSlotCountBits > 0 && kOccupiedBuildingSlotCountBits <= 8, "Invalid kOccupiedBuildingSlotCountBitss value");

    return get_slot_count().and_then([this, newCount](uint8_t slotCount) -> std::expected<void, building_data::BuildingError> {
        [[unlikely]] if (newCount > slotCount)
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewOccupiedCountExceededCurrentSlotCount});
        write_bits<uint8_t, kOccupiedBuildingSlotCountOffset, kOccupiedBuildingSlotCountBits>(newCount);
        return {};
    }).or_else([](building_data::BuildingError error) -> std::expected<void, building_data::BuildingError> { return std::unexpected<building_data::BuildingError>(error); });
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline std::expected<ElderTreetopIndex, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::get_elder_treetop_index() const
{
    static_assert(kTreetopIndexBits > 0 && kTreetopIndexBits <= 8, "Invalid kTreetopIndexBits value");
    static_assert(kMaxBuildingSlotCount > 0 && kMaxBuildingSlotCount < static_cast<uint8_t>(ElderTreetopIndex::kNotPresent) - 1, "Invalid kMaxBuildingSlotCount value");

    const ElderTreetopIndex index = read_bits<ElderTreetopIndex, kTreetopIndexOffset, kTreetopIndexBits>();
    if (index != ElderTreetopIndex::kNotPresent) {
        const auto slotcount = get_slot_count();
        [[unlikely]] if (!slotcount.has_value())
            return std::unexpected(slotcount.error());

        [[unlikely]] if (static_cast<uint8_t>(index) >= slotcount.value())
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kIndexExceededCurrentSlotCount});
    }
    return std::expected<ElderTreetopIndex, building_data::BuildingError>{index};
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
inline std::expected<void, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_elder_treetop_index(ElderTreetopIndex newIndex)
{
    static_assert(kTreetopIndexBits > 0 && kTreetopIndexBits <= 8, "Invalid kTreetopIndexBits value");
    static_assert(kMaxBuildingSlotCount > 0 && kMaxBuildingSlotCount < static_cast<uint8_t>(ElderTreetopIndex::kNotPresent) - 1, "Invalid kMaxBuildingSlotCount value");

    if (newIndex != ElderTreetopIndex::kNotPresent) {
        const auto slotCountResult = get_slot_count();
        if (!slotCountResult.has_value())
            return std::unexpected(slotCountResult.error());

        if (static_cast<uint8_t>(newIndex) >= slotCountResult.value())
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kIndexExceededCurrentSlotCount});
    }

    write_bits<ElderTreetopIndex, kTreetopIndexOffset, kTreetopIndexBits>(newIndex);
    return {};
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] std::expected<std::vector<building_data::Building>, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::get_occupied_building_slots() const
{
    static_assert(kBuildingSlotBits > 0 && kBuildingSlotBits <= 8, "Invalid kBuildingBits value");

    const auto buildingCount = get_occupied_slot_count();
    if (!buildingCount.has_value())
        return std::unexpected(buildingCount.error());
    
    if (buildingCount.value() == 0)
        return std::expected<std::vector<building_data::Building>, building_data::BuildingError>(std::vector<building_data::Building>{});

    if (buildingCount.value() == 1)
        // Fast path for a single building
        return std::expected<std::vector<building_data::Building>, building_data::BuildingError>(
            std::vector<building_data::Building>{read_bits<building_data::Building, kBuildingSlotsOffset, kBuildingSlotBits>()});

    std::vector<building_data::Building> result;

    return read_bits<std::vector<building_data::Building>, kBuildingSlotsOffset, kBuildingSlotBits>(buildingCount.value())
        .transform_error([](game_data::ReadWriteError error)
            { return building_data::BuildingError{static_cast<building_data::BuildingError::Code>(error.code)}; });
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
std::expected<void, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_buildings(const std::vector<building_data::Building> &newBuildings)
{
    static_assert(kBuildingSlotBits > 0 && kBuildingSlotBits <= 8, "Invalid kBuildingBits value");

    const uint8_t newOccupiedBuildingSlotCount = newBuildings.size();

    if (newOccupiedBuildingSlotCount == 0) {
        return set_occupied_slot_count(0);
    }

    const auto result = set_occupied_slot_count(newOccupiedBuildingSlotCount);
    [[unlikely]] if (!result.has_value())
        return result;

    if (newOccupiedBuildingSlotCount == 1) {
        // Fast path for a single building
        write_bits<building_data::Building, kBuildingSlotsOffset, kBuildingSlotBits>(newBuildings[0]);
        return {};
    }
    

    return write_bits<std::vector<building_data::Building>, kBuildingSlotsOffset, kBuildingSlotBits>(newOccupiedBuildingSlotCount, newBuildings)
        .transform_error([](game_data::ReadWriteError error)
            { return building_data::BuildingError{static_cast<building_data::BuildingError::Code>(error.code)}; });
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
std::expected<void, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_buildings(const std::vector<building_data::IndexBuildingPair> &newIndexBuildingPairs)
{
    static_assert(kBuildingSlotBits > 0 && kBuildingSlotBits <= 8, "Invalid kBuildingBits value");
    static_assert(kBuildingSlotsBits > 0 && kBuildingSlotsBits <= 32);
    
    [[unlikely]] if (newIndexBuildingPairs.empty())
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kSetZeroBuildings});

    const auto occupiedCount = get_occupied_slot_count();
    [[unlikely]] if (!occupiedCount.has_value())
        return std::unexpected(occupiedCount.error());

    std::bitset<kMaxBuildingSlotCount> seen;
    uint32_t buildingSlotBits = read_bits<uint32_t, kBuildingSlotsOffset, kBuildingSlotsBits>();
    for (const auto& pair : newIndexBuildingPairs) {
        [[unlikely]] if (pair.index > occupiedCount.value())
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kIndexExceededOccupiedSlotCount});

        [[unlikely]] if (seen.test(pair.index))
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kDuplicateIndices});

        seen.set(pair.index);

        static constexpr uint32_t kBuildingSlotMask = (1u << kBuildingSlotBits) - 1;
        // Clear the old value at this slot
        buildingSlotBits &= ~(kBuildingSlotMask << (pair.index * kBuildingSlotBits));
        // Set the new value at this slot
        buildingSlotBits |= (static_cast<uint32_t>(pair.building) & kBuildingSlotMask) << (pair.index * kBuildingSlotBits);
    }

    write_bits<uint32_t, kBuildingSlotsOffset, kBuildingSlotsBits>(buildingSlotBits);
    return {};
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
std::expected<void, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::add_buildings(const std::vector<building_data::Building> &newBuildings)
{
    static_assert(kBuildingSlotBits > 0 && kBuildingSlotBits <= 8, "Invalid kBuildingSlotBits value");
    
    const uint8_t newBuildingCount = newBuildings.size();

    [[unlikely]] if (newBuildingCount == 0)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kAddZeroBuildings});

    const auto oldOccupiedBuildingSlotCount = get_occupied_slot_count();
    [[unlikely]] if (!oldOccupiedBuildingSlotCount.has_value())
        return std::unexpected(oldOccupiedBuildingSlotCount.error());

    const std::expected<void, building_data::BuildingError> setOccupiedCountResult = set_occupied_slot_count(oldOccupiedBuildingSlotCount.value() + newBuildingCount);
    [[unlikely]] if (!setOccupiedCountResult.has_value())
        return setOccupiedCountResult;

    const uint8_t offset = kBuildingSlotBits * oldOccupiedBuildingSlotCount.value() + kBuildingSlotsOffset;
    if (newBuildingCount == 1)
        return write_bits<building_data::Building, kBuildingSlotBits>(newBuildings[0], offset)
            .transform_error([](game_data::ReadWriteError error)
                { return building_data::BuildingError{static_cast<building_data::BuildingError::Code>(error.code)}; });

    return write_bits<std::vector<building_data::Building>, kBuildingSlotBits>(newBuildingCount, newBuildings, offset)
        .transform_error([](game_data::ReadWriteError error)
            { return building_data::BuildingError{static_cast<building_data::BuildingError::Code>(error.code)}; });
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
std::expected<void, building_data::BuildingError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::remove_buildings(const std::vector<uint8_t> &indices)
{
    static_assert(kBuildingSlotBits > 0 && kBuildingSlotBits <= 8, "Invalid kBuildingSlotBits value");

    const uint8_t removalCount = indices.size();
    [[unlikely]] if (removalCount == 0)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kRemoveZeroBuildings});

    const auto occupiedBuildingSlots = get_occupied_building_slots();
    [[unlikely]] if (!occupiedBuildingSlots.has_value())
        return std::unexpected(occupiedBuildingSlots.error());

    const uint8_t buildingCount = occupiedBuildingSlots.value().size();
    [[unlikely]] if (removalCount > buildingCount)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kBuildingUnderflow});

    std::vector<uint8_t> sortedIndices = indices;
    std::sort(sortedIndices.begin(), sortedIndices.end());
    [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end())
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kDuplicateIndices});

    [[unlikely]] if (sortedIndices.back() >= buildingCount)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kIndexExceededCurrentSlotCount});

    // Build the new building slot vector by skipping sorted indices in a single pass
    std::vector<building_data::Building> newBuildings;
    newBuildings.reserve(buildingCount - sortedIndices.size());
    size_t removeIdx = 0;
    for (uint8_t i = 0; i < buildingCount; ++i) {
        // If current i matches the next index to remove, skip it
        if (removeIdx < sortedIndices.size() && i == sortedIndices[removeIdx]) {
            ++removeIdx;
            continue;
        }
        newBuildings.push_back(occupiedBuildingSlots.value()[i]);
    }

    return set_buildings(newBuildings); 
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
template<token_data::Token token>
[[nodiscard]] inline std::expected<uint8_t, TokenError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::get_token_count() const
{
    constexpr TokenDataInfo kTokenDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token)];

    const uint8_t count = read_bits<uint8_t, kTokenDataInfo.offset + kTokenDataOffset, kTokenDataInfo.width>();

    [[unlikely]] if (count > kTokenDataInfo.maxCount)
        return std::unexpected(TokenError{TokenError::Code::kCountExceededMaximumCount});

    return std::expected<uint8_t, TokenError>{count};
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
template <token_data::Token token, uint8_t newCount>
inline void Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_token_count()
{
    constexpr TokenDataInfo kTokenDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token)];
    static_assert(newCount <= kTokenDataInfo.maxCount, "Cannot set token count above maximum for token type");

    write_bits<uint8_t, kTokenDataInfo.offset + kTokenDataOffset, kTokenDataInfo.width>(newCount);
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
template <token_data::Token token>
inline std::expected<void, TokenError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_token_count(uint8_t newCount)
{
    constexpr TokenDataInfo kTokenDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token)];
    [[unlikely]] if (newCount > kTokenDataInfo.maxCount)
        return std::unexpected(TokenError{TokenError::Code::kNewCountExceededMaximumCount});

    write_bits<uint8_t, kTokenDataInfo.offset + kTokenDataOffset, kTokenDataInfo.width>(newCount);
    return {};
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline bool Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::contains_plot() const
{
    constexpr TokenDataInfo kBombPlotDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token_data::Token::kBombPlot)];
    constexpr TokenDataInfo kSnarePlotDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token_data::Token::kSnarePlot)];
    constexpr TokenDataInfo kExtorsionPlotDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token_data::Token::kExtortionPlot)];
    constexpr TokenDataInfo kRaidPlotDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token_data::Token::kRaidPlot)];

    constexpr uint8_t totalWidth =
        kBombPlotDataInfo.width +
        kSnarePlotDataInfo.width +
        kExtorsionPlotDataInfo.width +
        kRaidPlotDataInfo.width;
    static_assert(totalWidth > 0 && totalWidth <= 8, "Invalid sum of the bit width of all plots");

    // Anything >0 = true when cast to bool
    return static_cast<bool>(read_bits<uint8_t, kTokenDataOffset + kBombPlotDataInfo.offset, totalWidth>() == 0);
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline bool Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::is_plot_face_down() const
{
    static_assert(kHiddenPlotToggleBits == 1, "Hidden plot toggle bits must equal 1");
    return read_bits<bool, kHiddenPlotToggleOffset, kHiddenPlotToggleBits>();
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
inline void Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_is_plot_face_down(bool newStatus)
{
    static_assert(kHiddenPlotToggleBits == 1, "Hidden plot toggle bits must equal 1");
    write_bits<bool, kHiddenPlotToggleOffset, kHiddenPlotToggleBits>(newStatus);
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline bool Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::is_lord_of_the_hundreds_warlord_present() const
{
    constexpr PawnDataInfo warlordPawnDataInfo = kPawnDataInfoField[static_cast<uint8_t>(faction_data::FactionID::kLordOfTheHundreds) + 1];
    static_assert(warlordPawnDataInfo.width == 1, "Warlord pawn data width must equal 1");

    return read_bits<bool, warlordPawnDataInfo.offset + kPawnDataOffset, warlordPawnDataInfo.width>();
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
inline void Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_is_lord_of_the_hundreds_warlord_present(bool newStatus)
{
    constexpr PawnDataInfo warlordPawnDataInfo = kPawnDataInfoField[static_cast<uint8_t>(faction_data::FactionID::kLordOfTheHundreds) + 1];
    static_assert(warlordPawnDataInfo.width == 1, "Warlord pawn data width must equal 1");

    write_bits<bool, warlordPawnDataInfo.offset + kPawnDataOffset, warlordPawnDataInfo.width>(newStatus);
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
template <faction_data::FactionID factionID>
[[nodiscard]] inline std::expected<uint8_t, PawnError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::get_pawn_count() const
{
    // Account for lord of the hundreds warlord taking an extra index
    constexpr uint8_t index = (factionID > faction_data::FactionID::kLordOfTheHundreds) ? static_cast<uint8_t>(factionID) + 1 : static_cast<uint8_t>(factionID);
    constexpr PawnDataInfo kPawnDataInfo = kPawnDataInfoField[index];

    if constexpr (factionID == faction_data::FactionID::kLordOfTheHundreds) {
        constexpr PawnDataInfo warlordDataInfo = kPawnDataInfoField[index + 1];
        constexpr uint8_t totalWidth = kPawnDataInfo.width + warlordDataInfo.width;
        static_assert(warlordDataInfo.width == 1, "Warlord pawn data width must equal 1");
        static_assert(totalWidth > 0 && totalWidth <= 8, "Invalid sum of max standard and warlord lord of the hundreds pawns");

        uint8_t combined = read_bits<uint8_t, kPawnDataInfo.offset + kPawnDataOffset, totalWidth>();
        const uint8_t genericPawnCount = combined & ((1 << kPawnDataInfo.width) - 1);
        const bool isWarlordPresent = static_cast<bool>(combined >> kPawnDataInfo.width);
        [[unlikely]] if (genericPawnCount > kPawnDataInfo.maxCount)
            return std::unexpected(PawnError{PawnError::Code::kCountExceededMaximumCount});

        return (isWarlordPresent) ? genericPawnCount + 1 : genericPawnCount;
    }

    return read_bits<uint8_t, kPawnDataInfo.offset + kPawnDataOffset, kPawnDataInfo.width>(); 
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
template<faction_data::FactionID factionID>
inline std::expected<void, PawnError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_pawn_count_generic(uint8_t newCount)
{
    // Account for lord of the hundreds warlord taking an extra index
    constexpr uint8_t index = (factionID > faction_data::FactionID::kLordOfTheHundreds) ? static_cast<uint8_t>(factionID) + 1 : static_cast<uint8_t>(factionID);
    constexpr PawnDataInfo kPawnDataInfo = kPawnDataInfoField[static_cast<uint8_t>(index)];
    
    [[unlikely]] if (newCount > kPawnDataInfo.maxCount)
        return std::unexpected(PawnError{PawnError::Code::kNewCountExceededMaximumCount});

    write_bits<uint8_t, kPawnDataInfo.offset + Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::kPawnDataOffset, kPawnDataInfo.width>(newCount);
    return {};
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
template <faction_data::FactionID factionID>
inline std::expected<void, PawnError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_pawn_count(uint8_t newCount)
{
    static_assert(factionID != faction_data::FactionID::kLordOfTheHundreds, "Incorrect override for setting lord of the hundreds pawn count");
    return set_pawn_count_generic<factionID>(newCount);
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
template<faction_data::FactionID factionID, bool isWarlordPresent>
inline std::expected<void, PawnError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_pawn_count(uint8_t newCount)
{
    static_assert(factionID == faction_data::FactionID::kLordOfTheHundreds, "Incorrect override for setting generic faction pawn count");
    if constexpr (isWarlordPresent) {
        constexpr PawnDataInfo kPawnDataInfo = kPawnDataInfoField[static_cast<uint8_t>(faction_data::FactionID::kLordOfTheHundreds)];
        constexpr PawnDataInfo warlordDataInfo = kPawnDataInfoField[static_cast<uint8_t>(faction_data::FactionID::kLordOfTheHundreds) + 1];
        constexpr uint8_t totalWidth = kPawnDataInfo.width + warlordDataInfo.width;
        static_assert(warlordDataInfo.width == 1, "Warlord pawn data width must equal 1");
        static_assert(totalWidth > 0 && totalWidth <= 8, "Invalid sum of max standard and warlord lord of the hundreds pawns");

        write_bits<uint8_t, kPawnDataInfo.offset + kPawnDataOffset, totalWidth>(
            (static_cast<uint8_t>(true) << kPawnDataInfo.width) | newCount
        );
        return {};
    } else {
        return set_pawn_count_generic<factionID>(newCount);
    }
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline bool Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::is_razed() const
{
    static_assert(kRazedBits == 1, "Razed bits must equal 1");

    return read_bits<bool, kRazedOffset, kRazedBits>();
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
inline void Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_is_razed(bool newStatus)
{
    static_assert(kRazedBits == 1, "Razed bits must equal 1");

    write_bits<bool, kRazedOffset, kRazedBits>(newStatus);
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline std::vector<landmark_data::Landmark> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::get_landmarks() const
{
    static_assert(kLandmarkBits > 0 && kLandmarkBits < 8, "Invalid kLandmarkBitsValue");
    uint8_t combined = read_bits<uint8_t, kLandMarkOffset, kLandmarkBits>();

    std::vector<landmark_data::Landmark> result;
    auto dispatch = [&combined, &result]<size_t... Is>(std::index_sequence<Is...>) {
        ((combined & (1 << Is) ? result.push_back(static_cast<landmark_data::Landmark>(Is)) : void()), ...);
    };
    dispatch(std::make_index_sequence<kLandmarkBits>{});
    return result;
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
[[nodiscard]] inline std::expected<bool, landmark_data::LandmarkError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::is_landmark_present(landmark_data::Landmark desiredLandmark) const
{
    static_assert(kLandmarkBits > 0 && kLandmarkBits < 8, "Invalid kLandmarkBitsValue");

    const uint8_t offset = static_cast<uint8_t>(desiredLandmark) + kLandMarkOffset;
    return read_bits<bool, 1>(offset)
        .transform_error([](game_data::ReadWriteError error)
            { return landmark_data::LandmarkError{static_cast<landmark_data::LandmarkError::Code>(error.code)}; });
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
std::expected<void, landmark_data::LandmarkError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_landmarks(const std::vector<landmark_data::LandmarkStatusPair> &newLandmarkStatusPairs)
{
    static_assert(kLandmarkBits > 0 && kLandmarkBits < 8, "Invalid kLandmarkBits value");
    static_assert(kLandmarkBits == landmark_data::kTotalLandmarks, "kLandMarkBits must be equal to kTotalLandmarks");

    [[unlikely]] if (newLandmarkStatusPairs.empty())
        return std::unexpected(landmark_data::LandmarkError{landmark_data::LandmarkError::Code::kSetZeroLandmarks});
    
    std::bitset<landmark_data::kTotalLandmarks> seen;
    uint8_t landmarkBits = read_bits<uint8_t, kLandMarkOffset, kLandmarkBits>();
    for (const auto &pair : newLandmarkStatusPairs)
    {
        [[unlikely]] if (seen.test(static_cast<size_t>(pair.landmark)))
            return std::unexpected(landmark_data::LandmarkError{landmark_data::LandmarkError::Code::kDuplicateLandmarks});

        seen.set(static_cast<size_t>(pair.landmark));

        if (pair.status) { landmarkBits |= (1 << static_cast<uint8_t>(pair.landmark)); } 
        else { landmarkBits &= ~(1 << static_cast<uint8_t>(pair.landmark)); }
    }

    write_bits<uint8_t, kLandMarkOffset, kLandmarkBits>(landmarkBits);
    return {};
}

template<ClearingType clearingTypeValue, uint8_t initialSlotCount, bool hasRuinInitially>
std::expected<void, landmark_data::LandmarkError> Clearing<clearingTypeValue, initialSlotCount, hasRuinInitially>::set_landmarks(const std::vector<landmark_data::Landmark> &newLandmarks)
{
    static_assert(kLandmarkBits > 0 && kLandmarkBits < 8, "Invalid kLandmarkBitsValue");
    static_assert(kLandmarkBits == landmark_data::kTotalLandmarks, "kLandmarkBits must be equal to kTotalLandmarks");

    [[unlikely]] if (newLandmarks.empty())
        return std::unexpected(landmark_data::LandmarkError{landmark_data::LandmarkError::Code::kSetZeroLandmarks});

    [[unlikely]] if (newLandmarks.size() > landmark_data::kTotalLandmarks)
        return std::unexpected(landmark_data::LandmarkError{landmark_data::LandmarkError::Code::kNewLandmarkCountExceedsMaxLandmarks});

    // surely the compiler will auto-unroll this
    std::bitset<kLandmarkBits> seen;
    uint8_t landmarkBits = 0;
    for (const auto &landmark : newLandmarks) {
        [[unlikely]] if (seen.test(static_cast<size_t>(landmark)))
            return std::unexpected(landmark_data::LandmarkError{landmark_data::LandmarkError::Code::kDuplicateLandmarks});

        seen.set(static_cast<size_t>(landmark));
        landmarkBits |= (1 << static_cast<uint8_t>(landmark));
    }
    write_bits<uint8_t, kLandMarkOffset, kLandmarkBits>(landmarkBits);

    return {};
}
} // clearing_data
} // board_data
} // game_data