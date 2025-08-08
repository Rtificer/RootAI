#include "../include/clearing_data.hpp"

namespace game_data
{
namespace board_data
{
namespace clearing_data
{
[[nodiscard]] inline std::expected<uint8_t, board_data::ConnectionError>  Clearing::get_clearing_connection_count() const
{ 
    const std::expected<uint8_t, board_data::ConnectionError> connectionCount = read_bits<uint8_t, kConnectedClearingsCountOffset, kConnectedClearingsCountBits>();

    [[unlikely]] if (connectionCount.value() > kMaxClearingConnections)
        return std::unexpected(board_data::ConnectionError{board_data::ConnectionError::Code::kOldCountExceededMaxConnections});

    return connectionCount; 
}

template <uint8_t newCount>
inline void Clearing::set_clearing_connection_count()
{
    static_assert(kMaxClearingConnections > 0 && kMaxClearingConnections < board_data::kTotalClearings, "Invalid kMaxClearingConnections value");
    static_assert(newCount <= kMaxClearingConnections, "New clearing connection count cannot exceed kMaxClearingConnections");

    write_bits<uint8_t, kConnectedClearingsCountOffset, kConnectedClearingsCountBits>(newCount);
}

inline std::expected<void, board_data::ConnectionError> Clearing::set_clearing_connection_count(uint8_t newCount)
{
    static_assert(kMaxClearingConnections > 0 && kMaxClearingConnections < board_data::kTotalClearings, "Invalid kMaxClearingConnections value");
    [[unlikely]] if (newCount > kMaxClearingConnections)
        return std::unexpected(board_data::ConnectionError{board_data::ConnectionError::Code::kNewCountExceededMaxConnections});

    write_bits<uint8_t, kConnectedClearingsCountOffset, kConnectedClearingsCountBits>(newCount);
    return {};
}

// hehe it actually works
template <size_t I, size_t N>
inline void Clearing::unroll_separate_connections(const std::array<uint8_t, N> &combinedArray, std::vector<ClearingConnection> &separatedVector) {
    const uint8_t index = combinedArray[I] & ((1 << kClearingConnectionIndexBits) - 1);
    const ClearingConnectionType connectionType = static_cast<ClearingConnectionType>(combinedArray[I] >> kClearingConnectionIndexBits);
    separatedVector.push_back({index, connectionType});
    if constexpr (I + 1 < N) {
        unroll_separate_connections<I + 1, N>(combinedArray, separatedVector);
    }
}

[[nodiscard]] std::expected<std::vector<ClearingConnection> , board_data::ConnectionError> Clearing::get_clearing_connections() const
{
    static_assert(kClearingConnectionBits > 0 && kClearingConnectionBits <= 8, "Invalid kClearingConnectionBits value");

    const auto connectionCount = get_clearing_connection_count();
    [[unlikely]] if (!connectionCount.has_value())
        return std::unexpected(connectionCount.error());

    [[unlikely]] if (connectionCount.value() == 0)
        return std::expected<std::vector<ClearingConnection> , board_data::ConnectionError>(std::vector<ClearingConnection>{});
    
    [[unlikely]] if (connectionCount.value() == 1) {
        // Fast path for a single connection
        const uint8_t combined = read_bits<uint8_t, kConnectedClearingsOffset, kClearingConnectionBits>();
        const uint8_t index = combined & ((1 << kClearingConnectionIndexBits) - 1);
        const ClearingConnectionType connectionType = static_cast<ClearingConnectionType>(combined >> kClearingConnectionIndexBits);
        return std::expected<std::vector<ClearingConnection>, board_data::ConnectionError>(std::vector<ClearingConnection>{{index, connectionType}});
    }

    std::expected<std::vector<ClearingConnection> , board_data::ConnectionError> result;
    // Compile-time dispatch for all possible connection counts (starting from 2)
    auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
        (((connectionCount.value() == (Ns + 2)) ?
            ([&] {
                constexpr size_t N = Ns + 2;
                std::array<uint8_t, N> combinedArray = read_bits<std::array<uint8_t, N>, kConnectedClearingsOffset, kClearingConnectionBits>();
                result.value().reserve(N);
                unroll_separate_connections<0, N>(combinedArray, result.value());
            }(), void()) : void()), ...);
    };
    dispatch(std::make_index_sequence<kMaxClearingConnections - 1>{});
    return result;
}

template <size_t I, size_t N>
inline void Clearing::unroll_separate_connections_filtered(const std::array<uint8_t, N> &combinedArray, std::vector<uint8_t> &separatedVector, ClearingConnectionType desiredConnectionType) {
    const uint8_t index = combinedArray[I] & ((1 << kClearingConnectionIndexBits) - 1);
    const ClearingConnectionType connectionType = static_cast<ClearingConnectionType>(combinedArray[I] >> kClearingConnectionIndexBits);
    if (connectionType == desiredConnectionType)
        separatedVector.push_back(index);
    if constexpr (I + 1 < N) {
        unroll_separate_connections_filtered<I + 1, N>(combinedArray, separatedVector, desiredConnectionType);
    }
}

[[nodiscard]] std::expected<std::vector<uint8_t>, board_data::ConnectionError> Clearing::get_clearing_connections(ClearingConnectionType connectionType) const {
    static_assert(kClearingConnectionBits > 0 && kClearingConnectionBits <= 8, "Invalid kClearingConnectionBits value");

    const auto connectionCount = get_clearing_connection_count();
    [[unlikely]] if (!connectionCount.has_value())
        return std::unexpected(connectionCount.error());

    [[unlikely]] if (connectionCount.value() == 0) 
        return std::expected<std::vector<uint8_t>, board_data::ConnectionError>(std::vector<uint8_t>{});
    
    if (connectionCount.value() == 1) {
        // Fast path for a single connection
        const uint8_t combined = read_bits<uint8_t, kConnectedClearingsOffset, kClearingConnectionBits>();
        const uint8_t index = combined & ((1 << kClearingConnectionIndexBits) - 1);
        const ClearingConnectionType type = static_cast<ClearingConnectionType>(combined >> kClearingConnectionIndexBits);
        if (type == connectionType)
            return std::expected<std::vector<uint8_t>, board_data::ConnectionError>(std::vector<uint8_t>{index});

        return std::expected<std::vector<uint8_t>, board_data::ConnectionError>(std::vector<uint8_t>{});
    }

    std::expected<std::vector<uint8_t>, board_data::ConnectionError> result;
    // Compile-time dispatch for all possible connection counts (starting from 2)
    auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
        (((connectionCount.value() == (Ns + 2)) ?
            ([&] {
                constexpr size_t N = Ns + 2;
                std::array<uint8_t, N> combinedArray = read_bits<std::array<uint8_t, N>, kConnectedClearingsOffset, kClearingConnectionBits>();
                unroll_separate_connections_filtered<0, N>(combinedArray, result.value(), connectionType);
            }(), void()) : void()), ...);
    };
    dispatch(std::make_index_sequence<kMaxClearingConnections - 1>{});
    return result;
}

template <size_t I, size_t N>
inline void Clearing::unroll_combine_clearing_connections(const std::vector<ClearingConnection> &separatedVector, std::array<uint8_t, N> &combinedArray)
{
    combinedArray[I] = (static_cast<uint8_t>(separatedVector[I].connectionType) << kClearingConnectionIndexBits) | separatedVector[I].clearingIndex;
    if constexpr (I + 1 < N) {
        unroll_combine_clearing_connections<I + 1, N>(separatedVector, combinedArray);
    }
}

std::expected<void, board_data::ConnectionError>  Clearing::set_clearing_connections(const std::vector<ClearingConnection> &newConnections)
{
    static_assert(kClearingConnectionBits > 0 && kClearingConnectionBits <= 8, "Invalid kClearingConnectionBits value");

    const uint8_t newConnectionCount = newConnections.size();

    [[unlikely]] if (newConnectionCount == 0) {
        set_clearing_connection_count<0>();
        return {};
    }

    [[unlikely]] if (newConnectionCount > kMaxClearingConnections)
        return std::unexpected(board_data::ConnectionError{board_data::ConnectionError::Code::kNewCountExceededMaxConnections});

    [[unlikely]] if (newConnectionCount == 1) {
        // Fast path for a single connection
        write_bits<uint8_t, kConnectedClearingsOffset, kClearingConnectionBits>(
            (static_cast<uint8_t>(newConnections[0].connectionType) << kClearingConnectionIndexBits) | 
            newConnections[0].clearingIndex);
        set_clearing_connection_count<1>();
        return {};
    }
    // Compile-time dispatch for all possible clearing connection counts (starting from 2)
    std::bitset<kMaxClearingConnections> seen;
    for (const auto& connection : newConnections) {
        [[unlikely]] if (seen.test(connection.clearingIndex))
            return std::unexpected(board_data::ConnectionError{board_data::ConnectionError::Code::kDuplicateIndices});

        seen.set(connection.clearingIndex);
    }
        
    auto dispatch = [this, newConnectionCount, &newConnections]<size_t... Ns>(std::index_sequence<Ns...>) {
        (((newConnectionCount == (Ns + 2)) ?
            ([this, &newConnections] {
                constexpr size_t N = Ns + 2;
                std::array<uint8_t, N> combinedArray;
                unroll_combine_clearing_connections<0, N>(newConnections, combinedArray);
                write_bits<std::array<uint8_t, N>, kConnectedClearingsOffset, kClearingConnectionBits>(combinedArray);
                set_clearing_connection_count<N>();
            }(), void()) : void()), ...);
    };
    dispatch(std::make_index_sequence<kMaxClearingConnections - 1>{});

    return {};
}

std::expected<void, board_data::ConnectionError> Clearing::add_clearing_connections(const std::vector <ClearingConnection> &newConnections)
{
    static_assert(kClearingConnectionBits > 0 && kClearingConnectionBits <= 8, "Invalid kClearingConnectionBits value");

    const uint8_t totalNewConnections = newConnections.size();
    [[unlikely]] if (totalNewConnections == 0)
        return std::unexpected(board_data::ConnectionError{board_data::ConnectionError::Code::kAddZeroConnections});
    
    const auto oldConnectionCount = get_clearing_connection_count();
    [[unlikely]] if (!oldConnectionCount.has_value())
        return std::unexpected(oldConnectionCount.error());

    std::optional<board_data::ConnectionError::Code> optErrorCode;
    if (totalNewConnections == 1)
    {
        auto offset_dispatch = [this, &optErrorCode, oldConnectionCount, newConnections]<size_t... Offsets>(std::index_sequence<Offsets...>) {
            (((oldConnectionCount.value() == Offsets) ?
                ([this, oldConnectionCount, newConnections, &optErrorCode] {
                    constexpr uint8_t offset = Offsets * kClearingConnectionBits + kConnectedClearingsOffset;
                    const auto result = set_clearing_connection_count(oldConnectionCount.value() + 1);
                    [[likely]] if (result.has_value()) {
                        write_bits<uint8_t, offset, kClearingConnectionBits>(
                            (static_cast<uint8_t>(newConnections[0].connectionType) << kClearingConnectionIndexBits) | newConnections[0].clearingIndex); 
                    } else { optErrorCode = result.error().code; }
            }(), void()) : void()), ...);
        };
        offset_dispatch(std::make_index_sequence<kMaxClearingConnections>{});
    }
    else
    {
        auto count_dispatch = [this, totalNewConnections, &newConnections, oldConnectionCount, &optErrorCode]<size_t... Ns>(std::index_sequence<Ns...>)
        {
            (((totalNewConnections == (Ns + 2)) ? ([&] {
                constexpr size_t N = Ns + 1;
                const auto result = set_clearing_connection_count(oldConnectionCount.value() + N);
                [[likely]] if (result.has_value()) {
                    const uint8_t offset = oldConnectionCount.value() * kClearingConnectionBits + kConnectedClearingsOffset;
                    std::array<uint8_t, N> combinedArray{};
                    unroll_combine_clearing_connections<0, N>(newConnections, combinedArray);
                    write_bits<std::array<uint8_t, N>, kClearingConnectionBits>(combinedArray, offset);                    
                } else { optErrorCode = result.error().code; }
            }(), void()) : void()), ...);
        };
        count_dispatch(std::make_index_sequence<kMaxClearingConnections - 1>{});
    }

    [[unlikely]] if (optErrorCode.has_value())
        return std::unexpected(board_data::ConnectionError{optErrorCode.value()});

    return {};
}

std::expected<void, board_data::ConnectionError> Clearing::remove_clearing_connections(const std::vector<uint8_t> &indices)
{
    static_assert(kClearingConnectionBits > 0 && kClearingConnectionBits <= 8, "Invalid kClearingConnectionBits value");

    const uint8_t removalCount = indices.size();
    [[unlikely]] if (removalCount == 0)
        return std::unexpected(board_data::ConnectionError{board_data::ConnectionError::Code::kRemoveZeroConnections});

    auto oldConnectionsResult = get_clearing_connections();
    if (!oldConnectionsResult.has_value())
        return std::unexpected(oldConnectionsResult.error());

    const uint8_t oldConnectionCount = oldConnectionsResult.value().size();

    [[unlikely]] if (removalCount > oldConnectionCount)
        return std::unexpected(board_data::ConnectionError{board_data::ConnectionError::Code::kConnectionCountUnderflow});

    std::vector<uint8_t> sortedIndices = indices;
    std::sort(sortedIndices.begin(), sortedIndices.end());
    [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end()) {
        return std::unexpected(board_data::ConnectionError{board_data::ConnectionError::Code::kDuplicateIndices});
    }

    [[unlikely]] if (sortedIndices.back() >= oldConnectionCount)
        return std::unexpected(board_data::ConnectionError{board_data::ConnectionError::Code::kIndexExceedsCurrentConnections});


    const uint8_t newConnectionCount = oldConnectionCount - removalCount;

    // Build the new connection vector by skipping sorted indices in a single pass
    std::vector<ClearingConnection> newConnections;
    newConnections.reserve(newConnectionCount);
    size_t removeIdx = 0;
    for (uint8_t i = 0; i < oldConnectionCount; ++i) {
        // If current i matches the next index to remove, skip it
        if (removeIdx < sortedIndices.size() && i == sortedIndices[removeIdx]) {
            ++removeIdx;
            continue;
        }
        newConnections.push_back(oldConnectionsResult.value()[i]);
    }

    return set_clearing_connections(newConnections);
}

[[nodiscard]] inline uint8_t Clearing::get_occupied_slot_count_unsafe() const
{
    static_assert(kBuildingSlotCountBits > 0 && kBuildingSlotCountBits <= 8, "Invalid kBuildingSlotCountBits value");
    return read_bits<uint8_t, kOccupiedBuildingSlotCountOffset, kOccupiedBuildingSlotCountBits>();
}   

[[nodiscard]] inline std::expected<uint8_t, building_data::BuildingError> Clearing::get_slot_count() const 
{
    static_assert(kBuildingSlotCountBits > 0 && kBuildingSlotCountBits <= 8, "Invalid kBuildingSlotCountBits value");

    const std::expected<uint8_t, building_data::BuildingError> count = read_bits<uint8_t, kBuildingSlotCountOffset, kBuildingSlotCountBits>();

    [[unlikely]] if (count.value() > kMaxBuildingSlotCount)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kSlotCountExceededMaximumSlotCount});

    [[unlikely]] if (count.value() < get_occupied_slot_count_unsafe())
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kSlotCountWasLessThanOccupiedSlotCount});
    
    return count;
}


[[nodiscard]] inline std::expected<uint8_t, building_data::BuildingError> Clearing::get_occupied_slot_count() const
{
    static_assert(kOccupiedBuildingSlotCountBits > 0 && kOccupiedBuildingSlotCountBits <= 8, "Invalid kOccupiedBuildingSlotCountBits value");
    const std::expected<uint8_t, building_data::BuildingError> occupiedSlots = read_bits<uint8_t, kOccupiedBuildingSlotCountOffset, kOccupiedBuildingSlotCountBits>();
    return get_slot_count().and_then([&](uint8_t totalSlots) -> std::expected<uint8_t, building_data::BuildingError> {
        [[unlikely]] if (occupiedSlots.value() > totalSlots)
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kOccupiedExceededCurrentSlotCount});
        return occupiedSlots.value();
    }).or_else([&](building_data::BuildingError error) -> 
    std::expected<uint8_t, building_data::BuildingError> { return std::unexpected<building_data::BuildingError>(error); });
}

[[nodiscard]] inline std::expected<uint8_t, building_data::BuildingError> Clearing::get_remaining_slot_count() const
{
    static_assert(kBuildingSlotCountBits + kOccupiedBuildingSlotCountBits > 0 && kBuildingSlotCountBits + kOccupiedBuildingSlotCountBits <= 8, "Invalid combined kBuildingSlotCountBits and kOccupiedBuildingSlotCountOffset value");

    uint8_t combined = read_bits<uint8_t, kBuildingSlotCountOffset, kBuildingSlotCountBits + kOccupiedBuildingSlotCountBits>();

    const uint8_t remainingSlots = (combined & ((1 << kBuildingSlotCountBits) - 1)) - (combined >> kBuildingSlotCountBits);
    [[unlikely]] if (remainingSlots > kMaxBuildingSlotCount)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewOccupiedCountExceededMaximumSlotCount});
    
    return remainingSlots;
}

template<uint8_t newCount>
inline std::expected<void, building_data::BuildingError> Clearing::set_slot_count()
{
    static_assert(kBuildingSlotCountBits > 0 && kBuildingSlotCountBits <= 8, "Invalid kBuildingSlotCountBits value");
    static_assert(newCount <= kMaxBuildingSlotCount, "newCount must not exceed max building slot count");

    return get_occupied_slot_count().and_then([&](uint8_t oldCount) -> std::expected<void, building_data::BuildingError> {
        [[unlikely]] if (newCount < oldCount)
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewSlotCountWasLessThanOccupiedSlotCount});

        write_bits<uint8_t, kBuildingSlotCountOffset, kBuildingSlotCountBits>(newCount); 
        return {};
    }).or_else([&](building_data::BuildingError error) -> 
    std::expected<void, building_data::BuildingError> { return std::unexpected<building_data::BuildingError>(error); });
}

inline std::expected<void, building_data::BuildingError> Clearing::set_slot_count(uint8_t newCount)
{
    static_assert(kBuildingSlotCountBits > 0 && kBuildingSlotCountBits <= 8, "Invalid kBuildingSlotCountBits value");
    [[unlikely]] if (newCount > kMaxBuildingSlotCount)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewSlotCountExceededMaximumSlotCount});

    return get_occupied_slot_count().and_then([&](uint8_t oldCount) -> std::expected<void, building_data::BuildingError> {
        [[unlikely]] if (newCount < oldCount)
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewSlotCountWasLessThanOccupiedSlotCount});

        write_bits<uint8_t, kBuildingSlotCountOffset, kBuildingSlotCountBits>(newCount); 
        return {};
    }).or_else([&](building_data::BuildingError error) -> 
    std::expected<void, building_data::BuildingError> { return std::unexpected<building_data::BuildingError>(error); });
}

inline std::expected<void, building_data::BuildingError> Clearing::set_occupied_slot_count(uint8_t newCount)
{
    static_assert(kOccupiedBuildingSlotCountBits > 0 && kOccupiedBuildingSlotCountBits <= 8, "Invalid kOccupiedBuildingSlotCountBitss value");

    return get_slot_count().and_then([&](uint8_t slotCount) -> std::expected<void, building_data::BuildingError> {
        [[unlikely]] if (newCount > slotCount)
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewOccupiedCountExceededCurrentSlotCount});
        write_bits<uint8_t, kOccupiedBuildingSlotCountOffset, kOccupiedBuildingSlotCountBits>(newCount);
        return {};
    }).or_else([&](building_data::BuildingError error) -> std::expected<void, building_data::BuildingError> { return std::unexpected<building_data::BuildingError>(error); });
}

[[nodiscard]] inline std::expected<ElderTreetopIndex, building_data::BuildingError> Clearing::get_elder_treetop_index() const
{
    static_assert(kTreetopIndexBits > 0 && kTreetopIndexBits <= 8, "Invalid kTreetopIndexBits value");
    static_assert(kMaxBuildingSlotCount > 0 && kMaxBuildingSlotCount < static_cast<uint8_t>(ElderTreetopIndex::kNotPresent) - 1, "Invalid kMaxBuildingSlotCount value");
    const std::expected<ElderTreetopIndex, building_data::BuildingError> index = read_bits<ElderTreetopIndex, kTreetopIndexOffset, kTreetopIndexBits>();
    if (index != ElderTreetopIndex::kNotPresent) {
        const auto slotcount = get_slot_count();
        [[unlikely]] if (!slotcount.has_value())
            return std::unexpected(slotcount.error());

        [[unlikely]] if (static_cast<uint8_t>(index.value()) >= slotcount.value())
            return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kIndexExceededCurrentSlotCount});
    }
    return index;
}

inline std::expected<void, building_data::BuildingError> Clearing::set_elder_treetop_index(ElderTreetopIndex newIndex)
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

[[nodiscard]] std::expected<std::vector<building_data::Building>, building_data::BuildingError> Clearing::get_occupied_building_slots() const
{
    static_assert(kBuildingSlotBits > 0 && kBuildingSlotBits <= 8, "Invalid kBuildingBits value");
    const auto buildingCount = get_occupied_slot_count();
    if (!buildingCount.has_value())
        return std::unexpected(buildingCount.error());
    
    if (buildingCount.value() == 0)
        return std::expected<std::vector<building_data::Building>, building_data::BuildingError>(std::vector<building_data::Building>{});

    if (buildingCount.value() == 1)
        // Fast path for a single connection
        return std::expected<std::vector<building_data::Building>, building_data::BuildingError>(
            std::vector<building_data::Building>{read_bits<building_data::Building, kBuildingSlotsOffset, kBuildingSlotBits>()});

    std::vector<building_data::Building> result;
    // Compile-time dispatch for all possible forest connection counts (starting from 2)
    auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
        (((buildingCount.value() == (Ns + 2)) ?
            ([&] {
                constexpr size_t N = Ns + 2;
                std::array<building_data::Building, N> tempArray = read_bits<std::array<building_data::Building, N>, kBuildingSlotsOffset, kBuildingSlotBits>();
                result.assign(tempArray.begin(), tempArray.end());
            }(), void()) : void()), ...);
    };
    dispatch(std::make_index_sequence<kMaxBuildingSlotCount - 1>{});
    return result;

}

std::expected<void, building_data::BuildingError> Clearing::set_buildings(const std::vector<building_data::Building> &newBuildings)
{
    static_assert(kBuildingSlotBits > 0 && kBuildingSlotBits <= 8, "Invalid kBuildingBits value");
    const uint8_t newOccupiedBuildingSlotCount = newBuildings.size();
    const auto buildingSlotCount = get_slot_count();
    [[unlikely]] if (!buildingSlotCount.has_value())
        return std::unexpected(buildingSlotCount.error());

    [[unlikely]] if (newOccupiedBuildingSlotCount == 0) {
        return set_occupied_slot_count(0);
    } else [[unlikely]] if (newOccupiedBuildingSlotCount == 1 && buildingSlotCount.value() > 0) {
        // Fast path for a single building
        write_bits<building_data::Building, kBuildingSlotsOffset, kBuildingSlotBits>(newBuildings[0]);
        return set_occupied_slot_count(1);
    } else [[likely]] if (newOccupiedBuildingSlotCount <= buildingSlotCount.value()) {
        // Compile-time dispatch for all possible building slot counts (starting from 2)
        auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) -> std::expected<void, building_data::BuildingError> {
            std::expected<void, building_data::BuildingError> result = std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewOccupiedCountExceededMaximumSlotCount});
            (((newOccupiedBuildingSlotCount == (Ns + 2)) ?
                ([&] {
                    constexpr size_t N = Ns + 2;
                    std::array<building_data::Building, N> tempArray{};
                    std::copy_n(newBuildings.begin(), N, tempArray.begin());
                    write_bits<std::array<building_data::Building, N>, kBuildingSlotsOffset, kBuildingSlotBits>(tempArray);
                    result = set_occupied_slot_count(N);
                }(), void()) : void()), ...);
            return result;
        };
        return dispatch(std::make_index_sequence<kMaxBuildingSlotCount - 1>{});
    } else {
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewOccupiedCountExceededCurrentSlotCount});
    }
}

std::expected<void, building_data::BuildingError> Clearing::set_buildings(const std::vector<building_data::IndexBuildingPair> &newIndexBuildingPairs)
{
    static_assert(kBuildingSlotBits > 0 && kBuildingSlotBits <= 8, "Invalid kBuildingBits value");
    
    [[unlikely]] if (newIndexBuildingPairs.empty())
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kSetZeroBuildings});

    const auto buildingSlotCount = get_slot_count();
    [[unlikely]] if (!buildingSlotCount.has_value())
        return std::unexpected(buildingSlotCount.error());

    std::bitset<kMaxBuildingSlotCount> seen;
    for (const auto& pair : newIndexBuildingPairs) {
        // Compile-time dispatch for each possible index
        auto dispatch = [this, &seen, pair]<size_t... Ns>(std::index_sequence<Ns...>) -> std::expected<void, building_data::BuildingError> {
            [[unlikely]] if (seen.test(pair.index))
                return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kDuplicateIndices});

            bool handled = false;
            (((pair.index == Ns) ?
                ([&] {
                    write_bits<building_data::Building, Ns * kBuildingSlotBits + kBuildingSlotsOffset, kBuildingSlotBits>(pair.building);
                    handled = true;
                }(), void()) : void()), ...);
            [[unlikely]] if (handled == false)
                return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kIndexExceededMaximumSlotCount});

            seen.set(pair.index);
            return {};
        };
        auto result = dispatch(std::make_index_sequence<kMaxBuildingSlotCount>{});

        if (result.has_value())
            return result;
    }
    return {};
}

std::expected<void, building_data::BuildingError> Clearing::add_buildings(const std::vector<building_data::Building> &newBuildings)
{
    const uint8_t newBuildingCount = newBuildings.size();

    [[unlikely]] if (newBuildingCount == 0)
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kAddZeroBuildings});

    const auto oldOccupiedBuildingSlotCount = get_occupied_slot_count();
    [[unlikely]] if (!oldOccupiedBuildingSlotCount.has_value())
        return std::unexpected(oldOccupiedBuildingSlotCount.error());

    const uint8_t offset = kBuildingSlotBits * oldOccupiedBuildingSlotCount.value() + kBuildingSlotsOffset;
    if (newBuildingCount == 1)
    {
        auto offset_dispatch = [&]<size_t... Offsets>(std::index_sequence<Offsets...>) -> std::expected<void, building_data::BuildingError> {
            bool handled = false;
            (((oldOccupiedBuildingSlotCount == Offsets) ?
                ([&] {
                    constexpr uint8_t offset = Offsets * kBuildingSlotBits + kBuildingSlotsOffset;
                    write_bits<building_data::Building, offset, kBuildingSlotBits>(newBuildings[0]);
                    set_occupied_slot_count(oldOccupiedBuildingSlotCount.value() + 1);
                    handled = true;
                }(), void()) : void()), ...);
            [[unlikely]] if (handled == false) 
                return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewOccupiedCountExceededMaximumSlotCount});

            return {};
        };
        return offset_dispatch(std::make_index_sequence<kMaxBuildingSlotCount>{});
    }

    // Error case already checked by calling get_occupied_building_slot_count earlier
    // I could do a nested dispatch, dispatch for size, then offset but idk if its worth it. If this comment is still here than its not
    auto count_dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) -> std::expected<void, building_data::BuildingError> {
        std::expected<void, building_data::BuildingError> result = std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kNewOccupiedCountExceededMaximumSlotCount});
        (((newBuildingCount == (Ns + 2)) ?
            ([&] {
                constexpr size_t N = Ns + 1;
                std::array<building_data::Building, N> tempArray{};
                std::copy_n(newBuildings.begin(), N, tempArray.begin());
                write_bits<std::array<building_data::Building, N>, kBuildingSlotBits>(tempArray, offset);
                result = set_occupied_slot_count(oldOccupiedBuildingSlotCount.value() + newBuildingCount);
            }(), void()) : void()), ...);
        return result;
    };
    return count_dispatch(std::make_index_sequence<kMaxBuildingSlotCount - 1 >  {});
}

std::expected<void, building_data::BuildingError> Clearing::remove_buildings(const std::vector<uint8_t> &indices)
{
    [[unlikely]] if (indices.empty())
        return std::unexpected(building_data::BuildingError{building_data::BuildingError::Code::kRemoveZeroBuildings});

    const auto buildingCount = get_occupied_slot_count();
    [[unlikely]] if (!buildingCount.has_value())
        return std::unexpected(buildingCount.error());

    std::vector<uint8_t> sortedIndices = indices;
    
    // Get the current building slot contents
    const auto occupiedBuildingSlots = get_occupied_building_slots();
    [[unlikely]] if (!occupiedBuildingSlots.has_value())
        return std::unexpected(occupiedBuildingSlots.error());

    // Build the new building slot vector by skipping sorted indices in a single pass
    std::vector<building_data::Building> newBuildings;
    newBuildings.reserve(buildingCount.value() - sortedIndices.size());
    size_t removeIdx = 0;
    for (uint8_t i = 0; i < buildingCount.value(); ++i) {
        // If current i matches the next index to remove, skip it
        if (removeIdx < sortedIndices.size() && i == sortedIndices[removeIdx]) {
            ++removeIdx;
            continue;
        }
        newBuildings.push_back(occupiedBuildingSlots.value()[i]);
    }

    return set_buildings(occupiedBuildingSlots.value()); 
}

template<token_data::Token token>
[[nodiscard]] inline std::expected<uint8_t, TokenError> Clearing::get_token_count() const
{
    constexpr TokenDataInfo kTokenDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token)];
    const std::expected<uint8_t, TokenError> count = read_bits<uint8_t, kTokenDataInfo.offset + kTokenDataOffset, kTokenDataInfo.width>();
    [[unlikely]] if (count.value() > kTokenDataInfo.maxCount)
        return std::unexpected(TokenError{TokenError::Code::kCountExceededMaximumCount});

    return count;
}

template <token_data::Token token, uint8_t newCount>
inline void Clearing::set_token_count()
{
    constexpr TokenDataInfo kTokenDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token)];
    static_assert(newCount <= kTokenDataInfo.maxCount, "Cannot set token count above maximum for token type");

    write_bits<uint8_t, kTokenDataInfo.offset + kTokenDataOffset, kTokenDataInfo.width>(newCount);
}

template <token_data::Token token>
inline std::expected<void, TokenError> Clearing::set_token_count(uint8_t newCount)
{
    constexpr TokenDataInfo kTokenDataInfo = kTokenDataInfoField[static_cast<uint8_t>(token)];
    [[unlikely]] if (newCount > kTokenDataInfo.maxCount)
        return std::unexpected(TokenError{TokenError::Code::kNewCountExceededMaximumCount});

    write_bits<uint8_t, kTokenDataInfo.offset + kTokenDataOffset, kTokenDataInfo.width>(newCount);
    return {};
}

[[nodiscard]] inline bool Clearing::contains_plot() const
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

[[nodiscard]] inline bool Clearing::is_plot_face_down() const
{
    static_assert(kHiddenPlotToggleBits == 1, "Hidden plot toggle bits must equal 1");
    return read_bits<bool, kHiddenPlotToggleOffset, kHiddenPlotToggleBits>();
}

inline void Clearing::set_is_plot_face_down(bool newStatus)
{
    static_assert(kHiddenPlotToggleBits == 1, "Hidden plot toggle bits must equal 1");
    write_bits<bool, kHiddenPlotToggleOffset, kHiddenPlotToggleBits>(newStatus);
}

[[nodiscard]] inline bool Clearing::is_lord_of_the_hundreds_warlord_present() const
{
    constexpr PawnDataInfo warlordPawnDataInfo = kPawnDataInfoField[static_cast<uint8_t>(faction_data::FactionID::kLordOfTheHundreds) + 1];
    static_assert(warlordPawnDataInfo.width == 1, "Warlord pawn data width must equal 1");

    return read_bits<bool, warlordPawnDataInfo.offset + kPawnDataOffset, warlordPawnDataInfo.width>();
}

inline void Clearing::set_is_lord_of_the_hundreds_warlord_present(bool newStatus)
{
    constexpr PawnDataInfo warlordPawnDataInfo = kPawnDataInfoField[static_cast<uint8_t>(faction_data::FactionID::kLordOfTheHundreds) + 1];
    static_assert(warlordPawnDataInfo.width == 1, "Warlord pawn data width must equal 1");

    write_bits<bool, warlordPawnDataInfo.offset + kPawnDataOffset, warlordPawnDataInfo.width>(newStatus);
}

template <faction_data::FactionID factionID>
[[nodiscard]] inline std::expected<uint8_t, PawnError> Clearing::get_pawn_count() const
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

// This function assumed lord of the hundreds warlord is not present
template<faction_data::FactionID factionID>
inline std::expected<void, PawnError> Clearing::set_pawn_count_generic(uint8_t newCount)
{
    // Account for lord of the hundreds warlord taking an extra index
    constexpr uint8_t index = (factionID > faction_data::FactionID::kLordOfTheHundreds) ? static_cast<uint8_t>(factionID) + 1 : static_cast<uint8_t>(factionID);
    constexpr PawnDataInfo kPawnDataInfo = kPawnDataInfoField[static_cast<uint8_t>(index)];
    
    [[unlikely]] if (newCount > kPawnDataInfo.maxCount)
        return std::unexpected(PawnError{PawnError::Code::kNewCountExceededMaximumCount});

    write_bits<uint8_t, kPawnDataInfo.offset + Clearing::kPawnDataOffset, kPawnDataInfo.width>(newCount);
    return {};
}

template <faction_data::FactionID factionID>
inline std::expected<void, PawnError> Clearing::set_pawn_count(uint8_t newCount)
{
    static_assert(factionID != faction_data::FactionID::kLordOfTheHundreds, "Incorrect override for setting lord of the hundreds pawn count");
    return set_pawn_count_generic<factionID>(newCount);
}

template<faction_data::FactionID factionID, bool isWarlordPresent>
inline std::expected<void, PawnError> Clearing::set_pawn_count(uint8_t newCount)
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

[[nodiscard]] inline ClearingType Clearing::get_clearing_type() const
{
    static_assert(kClearingTypeBits > 0 && kClearingTypeBits <= 8, "Invalid kClearingTypeBits value");

    return read_bits<ClearingType, kClearingTypeOffset, kClearingTypeBits>();
}

inline void Clearing::set_clearing_type(ClearingType newType)
{
    static_assert(kClearingTypeBits > 0 && kClearingTypeBits <= 8, "Invalid kClearingTypeBits value");

    write_bits<ClearingType, kClearingTypeOffset, kClearingTypeBits>(newType);
}

[[nodiscard]] inline bool Clearing::is_razed() const
{
    static_assert(kRazedBits == 1, "Razed bits must equal 1");

    return read_bits<bool, kRazedOffset, kRazedBits>();
}

inline void Clearing::set_is_razed(bool newStatus)
{
    static_assert(kRazedBits == 1, "Razed bits must equal 1");

    write_bits<bool, kRazedOffset, kRazedBits>(newStatus);
}

[[nodiscard]] inline std::vector<landmark_data::Landmark> Clearing::get_landmarks() const
{
    static_assert(kLandmarkBits > 0 && kLandmarkBits < 8, "Invalid kLandmarkBitsValue");
    uint8_t combined = read_bits<uint8_t, kLandMarkOffset, kLandmarkBits>();

    std::vector<landmark_data::Landmark> result;
    auto dispatch = [&]<size_t... Is>(std::index_sequence<Is...>) {
        ((combined & (1 << Is) ? result.push_back(static_cast<landmark_data::Landmark>(Is)) : void()), ...);
    };
    dispatch(std::make_index_sequence<kLandmarkBits>{});
    return result;
}

[[nodiscard]] inline bool Clearing::is_landmark_present(landmark_data::Landmark desiredLandmark) const
{
    static_assert(kLandmarkBits > 0 && kLandmarkBits < 8, "Invalid kLandmarkBitsValue");

    bool result = false;
    auto dispatch = [&]<size_t... Offsets>(std::index_sequence<Offsets...>)
    {
        (((static_cast<uint8_t>(desiredLandmark) == Offsets) ? ([&] {
            constexpr uint8_t offset = Offsets + kLandMarkOffset;
            result = read_bits<bool, offset, 1>();
        }(), void()) : void()),...);
    };
    dispatch(std::make_index_sequence<kLandmarkBits>{});
    return result;
}

std::expected<void, landmark_data::LandmarkError> Clearing::set_landmarks(const std::vector<landmark_data::LandmarkStatusPair> &newLandmarkStatusPairs)
{
    static_assert(kLandmarkBits > 0 && kLandmarkBits < 8, "Invalid kLandmarkBits value");
    static_assert(kLandmarkBits == landmark_data::kTotalLandmarks, "kLandMarkBits must be equal to kTotalLandmarks");

    [[unlikely]] if (newLandmarkStatusPairs.empty())
        return std::unexpected(landmark_data::LandmarkError{landmark_data::LandmarkError::Code::kSetZeroLandmarks});
    
    std::bitset<landmark_data::kTotalLandmarks> seen;
    for (const auto &pair : newLandmarkStatusPairs)
    {
        [[unlikely]] if (seen.test(static_cast<size_t>(pair.landmark)))
            return std::unexpected(landmark_data::LandmarkError{landmark_data::LandmarkError::Code::kDuplicateLandmarks});

        seen.set(static_cast<size_t>(pair.landmark));
        auto dispatch = [&]<size_t... Offsets>(std::index_sequence<Offsets...>)
        {
            (((static_cast<uint8_t>(pair.landmark) == Offsets) ? ([&] {
                constexpr uint8_t offset = Offsets + kLandMarkOffset;
                write_bits<bool, offset, 1>(pair.status);
            }(), void()) : void()),...);
        };
        dispatch(std::make_index_sequence<kLandmarkBits>{});
    }

    return {};
}

std::expected<void, landmark_data::LandmarkError> Clearing::set_landmarks(const std::vector<landmark_data::Landmark> &newLandmarks)
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