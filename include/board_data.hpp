#pragma once

#include "game_data.hpp"

#include <cstdint>
#include <array>
#include <string_view>
#include <expected>
#include <vector>
#include <optional>
#include <bitset>
#include <algorithm>

namespace game_data
{
namespace board_data
{
static constexpr uint8_t kTotalClearings = 12;
static constexpr uint8_t kTotalForests = 12;

enum class BoardType : uint8_t {
    kAutumn,
    kWinter,
    kLake,
    kMountain
};

static constexpr uint8_t kTotalBoardTypes = 4;

struct ConnectionError {
    enum class Code : uint8_t {
        kNewCountExceededMaxConnections,
        kOldCountExceededMaxConnections,
        kDuplicateIndices,
        kAddZeroConnections,
        kRemoveZeroConnections,
        kIndexExceedsCurrentConnections,
        kConnectionCountUnderflow,
        kUnknownError
    } code;

    static constexpr std::array<std::string_view, 8> kMessages = {
        "New connection count exceeded maximum connection count",
        "Old clearing connection count exceeded maximum connection count",
        "Duplicate indices cannot be passed as function args",
        "Cannot add zero new connections",
        "Cannot remove zero new connections",
        "Desired index exceeded current connection count",
        "Cannot remove more connections then are currently present",
        "Unknown error"
    };

    [[nodiscard]] static std::string_view to_string(Code code) {
        uint8_t idx = static_cast<uint8_t>(code);
        if (idx < kMessages.size()) return kMessages[idx];
        return kMessages.back();
    }

    [[nodiscard]] std::string_view message() const { return to_string(code); }
};

template <uint8_t connectionCountBits, uint8_t connectionCountOffset, uint8_t maxConnections, size_t dataSize>
[[nodiscard]] inline std::expected<uint8_t, ConnectionError> get_basic_connection_count(const std::array<uint8_t, dataSize> &data)
{
    static_assert(connectionCountBits > 0 && connectionCountBits <= 8, "Invalid connectionCountBits value");
    const std::expected<uint8_t, ConnectionError> count = game_data::read_bits<uint8_t, dataSize, connectionCountOffset, connectionCountBits>(data);

    [[unlikely]] if (count.value() > maxConnections)
        return std::unexpected(ConnectionError{ConnectionError::Code::kOldCountExceededMaxConnections});

    return count;
}

template <uint8_t connectionCountBits, uint8_t connectionCountOffset, uint8_t maxConnections, size_t dataSize, uint8_t newCount>
inline void set_basic_connection_count(std::array<uint8_t, dataSize> &data)
{
    static_assert(connectionCountBits > 0 && connectionCountBits <= 8, "Invalid connectionCountBits value");
    static_assert(newCount <= maxConnections, "newCount must not exceed maximum connections");

    game_data::write_bits<uint8_t, dataSize, connectionCountOffset, connectionCountBits>(data, newCount);
}

template <uint8_t connectionCountBits, uint8_t connectionCountOffset, uint8_t maxConnections, size_t dataSize>
[[nodiscard]] inline std::expected<void, ConnectionError> set_basic_connection_count(std::array<uint8_t, dataSize> &data, uint8_t newCount)
{
    static_assert(connectionCountBits > 0 && connectionCountBits <= 8, "Invalid connectionCountBits value");
    [[unlikely]] if (newCount > maxConnections)
        return std::unexpected(ConnectionError{ConnectionError::Code::kNewCountExceededMaxConnections});

    game_data::write_bits<uint8_t, dataSize, connectionCountOffset, connectionCountBits>(data, newCount);
    return {};
}

template <uint8_t connectionCountBits, uint8_t connectionCountOffset, uint8_t maxConnections, uint8_t connectionIndexBits, uint8_t connectionIndicesOffset, size_t dataSize> 
[[nodiscard]] std::expected<std::vector<uint8_t>, ConnectionError> get_basic_connections(const std::array<uint8_t, dataSize> &data)
{
    static_assert(connectionIndexBits > 0 && connectionIndexBits <= 8, "Invalid connectionIndexBits value");

    const auto connectionCount = get_basic_connection_count<connectionCountBits, connectionCountOffset, maxConnections, dataSize>(data);
    [[unlikely]] if (!connectionCount.has_value())
        return std::unexpected(connectionCount.error());

    [[unlikely]] if (connectionCount.value() == 0)
        return std::expected<std::vector<uint8_t>, ConnectionError>(std::vector<uint8_t>{});

    [[unlikely]] if (connectionCount.value() == 1)
        // Fast path for a single connection
        return std::expected<std::vector<uint8_t>, ConnectionError>(std::vector<uint8_t>{game_data::read_bits<uint8_t, dataSize, connectionIndicesOffset, connectionIndexBits>(data)});
    
    std::expected<std::vector<uint8_t>, ConnectionError> result;
    // Compile-time dispatch for all possible connection counts (starting from 2)
    auto dispatch = [&]<size_t... Ns>(std::index_sequence<Ns...>) {
        (((connectionCount.value() == (Ns + 2)) ?
            ([&] {
                constexpr size_t N = Ns + 2;
                std::array<uint8_t, N> tempArray = game_data::read_bits<std::array<uint8_t, N>, dataSize, connectionIndicesOffset, connectionIndexBits>(data);
                result.value().assign(tempArray.begin(), tempArray.end());
            }(), void()) : void()), ...);
    };
    dispatch(std::make_index_sequence<maxConnections - 1>{});
    return result;
}

template <uint8_t connectionCountBits, uint8_t connectionCountOffset, uint8_t maxConnections, 
          uint8_t connectionIndexBits, uint8_t connectionIndicesOffset, size_t dataSize>
[[nodiscard]] std::expected<void, ConnectionError> set_basic_connections(std::array<uint8_t, dataSize> &data, const std::vector<uint8_t> &newConnections)
{
    static_assert(connectionIndexBits > 0 && connectionIndexBits <= 8, "Invalid connectionIndexBits value");

    const uint8_t newConnectionCount = newConnections.size();

    [[unlikely]] if (newConnectionCount == 0) {
        set_basic_connection_count<connectionCountBits, connectionCountOffset, maxConnections, dataSize, 0>(data);
        return {};
    };

    [[unlikely]] if (newConnectionCount > maxConnections)
        return std::unexpected(ConnectionError{ConnectionError::Code::kNewCountExceededMaxConnections});
    
    [[unlikely]] if (newConnectionCount == 1) {
        // Fast path for a single connection
        game_data::write_bits<uint8_t, dataSize, connectionIndicesOffset, connectionIndexBits>(data, newConnections[0]);
        set_basic_connection_count<connectionCountBits, connectionCountOffset, maxConnections, dataSize, 1>(data);
        return {};
    }

    // Compile-time dispatch for all possible connection counts (starting from 2)
    std::bitset<maxConnections> seen;
    for (const auto& connection : newConnections) {
        [[unlikely]] if (seen.test(connection))
            return std::unexpected(ConnectionError{ConnectionError::Code::kDuplicateIndices});

        seen.set(connection);
    }

    auto dispatch = [newConnectionCount, &newConnections, &data]<size_t... Ns>(std::index_sequence<Ns...>) {
        (((newConnectionCount == (Ns + 2)) ?
            ([&newConnections, &data] {
                constexpr size_t N = Ns + 2;
                std::array<uint8_t, N> tempArray{};
                std::copy_n(newConnections.begin(), N, tempArray.begin());
                game_data::write_bits<std::array<uint8_t, N>, dataSize, connectionIndicesOffset, connectionIndexBits>(data, tempArray);
                set_basic_connection_count<connectionCountBits, connectionCountOffset, maxConnections, dataSize, N>(data);
            }(), void()) : void()), ...);
    };
    dispatch(std::make_index_sequence<maxConnections - 1>{});

    return {};
}

template <uint8_t connectionCountBits, uint8_t connectionCountOffset, uint8_t maxConnections, uint8_t connectionIndexBits, uint8_t connectionIndicesOffset, size_t dataSize>
[[nodiscard]] std::expected<void, ConnectionError> add_basic_connections(std::array<uint8_t, dataSize> &data, const std::vector<uint8_t> &newConnections)
{
    static_assert(connectionIndexBits > 0 && connectionIndexBits <= 8, "Invalid connectionIndexBits value");

    const uint8_t totalNewConnections = newConnections.size();
    [[unlikely]] if (totalNewConnections == 0)
        return std::unexpected(ConnectionError{ConnectionError::Code::kAddZeroConnections});
    
    const auto oldConnectionCount = get_basic_connection_count<connectionCountBits, connectionCountOffset, maxConnections, dataSize>(data);
    [[unlikely]] if (!oldConnectionCount.has_value())
        return std::unexpected(oldConnectionCount.error());

    
    std::optional<ConnectionError::Code> optErrorCode;
    if (totalNewConnections == 1)
    {
        auto offset_dispatch = [oldConnectionCount, &data, newConnections, &optErrorCode]<size_t... Offsets>(std::index_sequence<Offsets...>) {
            (((oldConnectionCount.value() == Offsets) ?
                ([&data, oldConnectionCount, newConnections, &optErrorCode] {
                    auto result = set_basic_connection_count<connectionCountBits, connectionCountOffset, maxConnections, dataSize>(data, oldConnectionCount.value() + 1);
                        [[likely]] if (result.has_value()) {
                            constexpr uint8_t offset = Offsets + connectionIndicesOffset;
                            game_data::write_bits<uint8_t, dataSize, offset, connectionIndexBits>(data, newConnections[0]);
                        } else { optErrorCode = result.error().code; }
                }(), void()) : void()), ...);
        };
        offset_dispatch(std::make_index_sequence<maxConnections>{});
    } 
    else 
    {

        std::bitset<maxConnections> seen;
        for (const auto& connection : newConnections) {
            [[unlikely]] if (seen.test(connection))
                return std::unexpected(ConnectionError{ConnectionError::Code::kDuplicateIndices});

            seen.set(connection);
        }

        auto count_dispatch = [totalNewConnections, &data, oldConnectionCount, &newConnections, &optErrorCode]<size_t... Ns>(std::index_sequence<Ns...>)
        {
            (((totalNewConnections == (Ns + 2)) ? ([&data, oldConnectionCount, &newConnections, &optErrorCode] {
                constexpr size_t N = Ns + 1;
                auto result = set_basic_connection_count<connectionCountBits, connectionCountOffset, maxConnections, dataSize>(data, oldConnectionCount.value() + N);
                    [[likely]] if (result.has_value()) {
                        const uint8_t offset = oldConnectionCount.value() + connectionIndicesOffset;
                        std::array<uint8_t, N> tempArray{};
                        std::copy_n(newConnections.begin(), N, tempArray.begin());
                        game_data::write_bits<std::array<uint8_t, N>, dataSize, connectionIndexBits>(data, tempArray, offset);
                    } else { optErrorCode = result.error().code; }
            }(), void()) : void()), ...);
        };
        count_dispatch(std::make_index_sequence<maxConnections - 1>{});
    }

    [[unlikely]] if (optErrorCode.has_value())
        return std::unexpected(ConnectionError{optErrorCode.value()});

    return {};
}

template <uint8_t connectionCountBits, uint8_t connectionCountOffset, uint8_t maxConnections, uint8_t connectionIndexBits, uint8_t connectionIndicesOffset, size_t dataSize>
std::expected<void, ConnectionError> remove_basic_connections(std::array<uint8_t, dataSize> &data, const std::vector<uint8_t> &indices)
{
    static_assert(connectionIndexBits > 0 && connectionIndexBits <= 8, "Invalid connectionIndexBits value");

    const uint8_t removalCount = indices.size();
    [[unlikely]] if (removalCount == 0)
        return std::unexpected(ConnectionError{ConnectionError::Code::kRemoveZeroConnections});

    const auto oldConnectionsResult = get_basic_connections<connectionCountBits, connectionCountOffset, maxConnections, connectionIndexBits, connectionIndicesOffset, dataSize>(data);
    if (!oldConnectionsResult.has_value())
        return std::unexpected(oldConnectionsResult.error());
    
    const uint8_t oldConnectionCount = oldConnectionsResult.value().size();

    [[unlikely]] if (removalCount > oldConnectionCount)
        return std::unexpected(ConnectionError{ConnectionError::Code::kConnectionCountUnderflow});

    std::vector<uint8_t> sortedIndices = indices;
    std::sort(sortedIndices.begin(), sortedIndices.end());
    [[unlikely]] if (std::adjacent_find(sortedIndices.begin(), sortedIndices.end()) != sortedIndices.end()) {
        return std::unexpected(ConnectionError{ConnectionError::Code::kDuplicateIndices});
    }

    [[unlikely]] if (sortedIndices.back() >= oldConnectionCount)
        return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceedsCurrentConnections});

    const uint8_t newConnectionCount = oldConnectionCount - removalCount;

    // Build the new connection vector by skipping sorted indices in a single pass
    std::vector<uint8_t> newConnections;
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

    return set_basic_connections<connectionCountBits, connectionCountOffset, maxConnections, connectionIndexBits, connectionIndicesOffset, dataSize>(data, newConnections);    
}

// template <BoardType boardType>
// class Board
// {
// };
} // board_data
} // game_data