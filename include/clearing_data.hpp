#pragma once

#include "token_data.hpp"
#include "board_data.hpp"
#include "game_data.hpp"

#include <cstdint>
#include <array>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <expected>
#include <bitset>

namespace game_data 
{
namespace board_data 
{
namespace clearing_data
{

namespace faction_data = ::game_data::faction_data;
namespace token_data = ::game_data::token_data;
namespace board_data = ::game_data::board_data;
namespace game_data = ::game_data;

namespace building_data
{
enum class Building : uint8_t
{
    kRuin,
    kWorkshop,
    kSawmill,
    kRecruiter,
    kRoost,
    kMouseBase,
    kFoxBase,
    kRabbitBase,
    kMouseGarden,
    kFoxGarden,
    kRabbitGarden,
    kCitadel,
    kMarket,
    kStronghold,
    kFigureAndTabletWaystation,
    kTabletAndJewelryWaystation,
    kJewelryAndFigureWaystation,
    kMaxBuildingIndex // for bounds checking
};

struct IndexBuildingPair 
{
    uint8_t index;
    Building building;
};

struct BuildingError {
    enum class Code : uint8_t {
        kOccupiedExceededMaximumSlotCount,
        kOccupiedExceededCurrentSlotCount,
        kNewOccupiedCountExceededMaximumSlotCount,
        kNewOccupiedCountExceededCurrentSlotCount,
        kSlotCountExceededMaximumSlotCount,
        kSlotCountWasLessThanOccupiedSlotCount,
        kNewSlotCountExceededMaximumSlotCount,
        kNewSlotCountWasLessThanOccupiedSlotCount,
        kIndexExceededCurrentSlotCount,
        kIndexExceededMaximumSlotCount,
        kIndexExceededOccupiedSlotCount,
        kDuplicateIndices,
        kSetZeroBuildings,
        kAddZeroBuildings,
        kRemoveZeroBuildings,
        kBuildingUnderflow,
        kBuildingSlotUnderflow,
        kUnknownError
    } code;

    static constexpr std::array<std::string_view, 18> kMessages = {
        "Occupied slot count exceeded maximum slot count",
        "Occupied slot count exceeded current slot count",
        "New occupied slot count exceeded maximum slot count",
        "New occupied slot count exceeded current slot count",
        "Slot count exceeded maximum slot count",
        "Slot count was less than occupied slot count",
        "New slot count exceeded maximum slot count",
        "New slot count was less than occupied slot count",
        "Slot index exceeded current slot count",
        "Slot index exceeded maximum slot count",
        "Slot index exceeded occupied slot count",
        "Duplicate indices cannot be passed as function args",
        "Cannot set zero new buildings",
        "Cannot add zero new buildings",
        "Cannot remove zero buildings",
        "Cannot remove more buildings then are currently present",
        "Cannot remove more building slots then are currently present",
        "Unknown error"
    };

    static std::string_view to_string(Code code) {
        uint8_t idx = static_cast<uint8_t>(code);
        if (idx < kMessages.size()) return kMessages[idx];
        return kMessages.back();
    }

    std::string_view message() const { return to_string(code); }
};
}

namespace landmark_data
{
enum class Landmark : uint8_t
{
    kBlackMarket,
    kFerry,
    kLegendaryForge,
    kLostCity,
    kTower,
};
static constexpr uint8_t kTotalLandmarks = 5;

struct LandmarkError {
    enum class Code : uint8_t {
        kNewLandmarkCountExceedsMaxLandmarks,
        kDuplicateLandmarks,
        kSetZeroLandmarks,
        kUnknownError
    } code;

    static constexpr std::array<std::string_view, 4> kMessages = {
        "New landmark count exceeds maximum landmarks as defined by kLandmarkBits",
        "Duplicate landmarks cannot be passed as function args",
        "Cannot set zero landmarks",
        "Unknown error"
    };

    static std::string_view to_string(Code code) {
        uint8_t idx = static_cast<uint8_t>(code);
        if (idx < kMessages.size()) return kMessages[idx];
        return kMessages.back();
    }

    std::string_view message() const { return to_string(code); }
};

struct LandmarkStatusPair
{
    Landmark landmark;
    bool status;
};
}

enum class ClearingConnectionType : uint8_t
{
    kNoConnection,
    kNormal,
    kByWater,
    kBlocked,
    kMaxConnectionIndex // for bounds checking
};

struct ClearingConnection
{
    uint8_t clearingIndex;
    ClearingConnectionType connectionType;
};

enum class ClearingType : uint8_t
{
    kNone,
    kMouse,
    kFox,
    kRabbit,
};

enum class ElderTreetopIndex : uint8_t
{
    k0,
    k1,
    k2,
    k3,
    k4, // There should only be 4 possible slots but just in case
    k5,
    k6,
    kNotPresent
};


struct TokenError {
    enum class Code : uint8_t {
        kCountExceededMaximumCount,
        kNewCountExceededMaximumCount,
        kUnknownError
    } code;

    static constexpr std::array<std::string_view, 3> kMessages = {
        "Token count exceeded maximum token count of that type",
        "New token count exceeded maximum token count of that type",
        "Unknown error"
    };

    static std::string_view to_string(Code code) {
        uint8_t idx = static_cast<uint8_t>(code);
        if (idx < kMessages.size()) return kMessages[idx];
        return kMessages.back();
    }

    std::string_view message() const { return to_string(code); }
};

struct PawnError {
    enum class Code : uint8_t {
        kCountExceededMaximumCount,
        kNewCountExceededMaximumCount,
        kUnknownError
    } code;

    static constexpr std::array<std::string_view, 3> kMessages = {
        "Pawn count exceeded maximum pawn count of that faction",
        "New pawn count exceeded maximum pawn count of that faction",
        "Unknown error"
    };

    static std::string_view to_string(Code code) {
        uint8_t idx = static_cast<uint8_t>(code);
        if (idx < kMessages.size()) return kMessages[idx];
        return kMessages.back();
    }

    std::string_view message() const { return to_string(code); }
};


class Clearing
{
    /*
    39 bits: Connected Clearings {
    Clearing Index (4 Bits)
    Connection (2 bits): {
        0 = Normal
        1 = By water
        2 = Blocked
    } x 6 for each possible connection
    + connected clearing count 3 bits
    }

    26 bits: Building Slots (4 slots times 5 bits for building type, plus 3 bits for occupied slot count, and 3 bits for total slot count) 
    { 
        Slot (6 bits): 
        { 
            Current Building (5 bits): 
            { 
                0 = No building, 
                2 = Ruin, 
                3 = Workshop, 
                4 = Sawmill, 
                5 = Roost, 
                6 = Mouse Base, 
                7 = Fox Base,
                8 = Rabbit Base, 
                9 = Mouse Garden, 
                10 = Fox Garden, 
                11 = Rabbit Garden, 
                12 = Citadel, 
                13 = Market, 
                14 = Stronghold, 
                15 = Figure / Tablet Waystation 
                16 = Tablet / Jewelry Waystation
                17 = Jewelry / Figure Waystation
                18-31 = unused
            }
        }
    } x 4 for each possible slot

    3 Bits: Elder Treetop Index (0-kMaxBuildingSlotIndex, 7 = Not present)

    28 Bits: Tokens {
        Bits 1-3: Wood (0-8 (9-15 unused))
        Bit 4: Keep (0-1)
        Bit 5: Sympathy (0-1)
        Bit 6: Mouse Trade Post (0-1)
        Bit 7: Fox Trade Post (0-1)
        Bit 8: Rabbit Trade Post (0-1)
        Bit 9: Bomb Plot (0-1)
        Bit 10-11: Snare Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bits 12-13: Extortion Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bits 14-15: Raid Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused) 
        Bit 16: Tunnel (0-1) 
        Bit 17: Mob (0-1) 
        Bit 18 Figure Value 1 (0-1) 
        Bit 19 Figure Value 2 (0-1) 
        Bits 20-21 Figure Value 3 (0-2, 3 = unused) 
        Bit 22: Tablet Value 1 (0-1) 
        Bit 23: Tablet Value 2 (0-1) 
        Bits 24-25: Tablet Value 3 (0-2, 3 = unused) 
        Bit 26: Jewelry Value 1 (0-1) 
        Bit 27: Jewelry Value 2 (0-1)
        Bit 28: Jewelry Value 3 (0-2, 3 = unused)
    }

    44 Bits Pawn Data {
        Bits 1-5: Marquise de Cat Pawns (0-25, 26-31 uneeded)
        Bits 6-10: Eyrie Dynasties Pawns (0-20, 21-31 uneeded)
        Bit 11: Vagabond 1 (0-1)
        Bit 12: Vagabond 2 (0-1)
        Bits 13-16: Woodland Alliance Pawns (0-10, 11-15 uneeded)
        Bits 17-20: Riverfolk Company Pawns (0-15)
        Bits 21-25: Lizard Cult Pawns (0-25, 26-31 uneeded)
        Bits 26-29: Corvid Conspiracy Pawns (0-15)
        Bits 30-34: Underground Duchy Pawns (0-20, 21-31 uneeded)
        Bits 35-39: Lord of the Hundreds Pawns (0-20, 21-31 uneeded)
        Bit 40: Lord of the Hundreds Warlord (0-1)
        Bits 41-44: Keepers in Iron Pawns (0-15)
    }

    2 Bits: Clearing Type {
        0 = Mouse
        1 = Fox
        2 = Rabbit
        3 = None
    }

    27 Bits: Connected Forests {
        4 Bits (Connected Forests Index)
    } x 6 for each possible forest connection + 3 Bits for Total Forest connections

    1 Bit: Razed? (0-1)

    5 Bits: Landmarks {
        Bit 1 = Black Market
        Bit 2 = Ferry
        Bit 3 = Legendary Forge
        Bit 4 = Lost City
        Bit 5 = Tower
    }
    */
public:
    // Clearing (
    //     const std::vector<ClearingConnection> &clearingConnections, 
    //     const std::vector<uint8_t> &forestConnections
    // )
    // {
    //     set_clearing_connections(clearingConnections);
    //     set_forest_connections(forestConnections);
    // };

    [[nodiscard]] inline std::expected<uint8_t, board_data::ConnectionError> get_clearing_connection_count() const;
    template <uint8_t newCount>
    void set_clearing_connection_count();
    inline std::expected<void, board_data::ConnectionError> set_clearing_connection_count(uint8_t newCount);

    [[nodiscard]] std::expected<std::vector<ClearingConnection> , board_data::ConnectionError> get_clearing_connections() const;
    [[nodiscard]] std::expected<std::vector<uint8_t> , board_data::ConnectionError> get_clearing_connections(ClearingConnectionType connectionType) const;
    std::expected<void, board_data::ConnectionError> set_clearing_connections(const std::vector<ClearingConnection> &newClearingConnections);
    std::expected<void, board_data::ConnectionError> add_clearing_connections(const std::vector <ClearingConnection> &newClearingConnections);
    std::expected<void, board_data::ConnectionError> remove_clearing_connections(const std::vector<uint8_t> &indices);

    [[nodiscard]] inline std::expected<uint8_t, building_data::BuildingError> get_slot_count() const;
    [[nodiscard]] inline std::expected<uint8_t, building_data::BuildingError> get_occupied_slot_count() const;
    [[nodiscard]] inline std::expected<uint8_t, building_data::BuildingError> get_remaining_slot_count() const;
    template<uint8_t newCount>
    inline std::expected<void, building_data::BuildingError> set_slot_count();
    inline std::expected<void, building_data::BuildingError> set_slot_count(uint8_t newCount);

    inline std::expected<void, building_data::BuildingError> set_occupied_slot_count(uint8_t newCount);
    
    [[nodiscard]] inline std::expected<ElderTreetopIndex, building_data::BuildingError> get_elder_treetop_index() const;
    inline std::expected<void, building_data::BuildingError> set_elder_treetop_index(ElderTreetopIndex newIndex);

    [[nodiscard]] std::expected<std::vector<building_data::Building>, building_data::BuildingError> get_occupied_building_slots() const;
    std::expected<void, building_data::BuildingError> set_buildings(const std::vector<building_data::Building> &newBuildings);
    std::expected<void, building_data::BuildingError> set_buildings(const std::vector<building_data::IndexBuildingPair> &newIndexBuildingPairs);
    std::expected<void, building_data::BuildingError> add_buildings(const std::vector<building_data::Building> &newBuildings);
    std::expected<void, building_data::BuildingError> remove_buildings(const std::vector<uint8_t> &indices);

    template <token_data::Token token>
    [[nodiscard]] inline std::expected<uint8_t, TokenError> get_token_count() const;
    template <token_data::Token token, uint8_t newCount>
    inline void set_token_count();
    template <token_data::Token token>
    inline std::expected<void, TokenError> set_token_count(uint8_t newCount);

    [[nodiscard]] inline bool contains_plot() const;
    
    [[nodiscard]] inline bool is_plot_face_down() const;
    inline void set_is_plot_face_down(bool newStatus);

    [[nodiscard]] inline bool is_lord_of_the_hundreds_warlord_present() const;
    inline void set_is_lord_of_the_hundreds_warlord_present(bool newStatus);

    template<faction_data::FactionID factionID>
    [[nodiscard]] inline std::expected<uint8_t, PawnError> get_pawn_count() const;
    template<faction_data::FactionID factionID>
    inline std::expected<void, PawnError> set_pawn_count(uint8_t newCount);
    template<faction_data::FactionID factionID, bool isWarlordPresent>
    inline std::expected<void, PawnError> set_pawn_count(uint8_t newCount);

    [[nodiscard]] inline ClearingType get_clearing_type() const;
    inline void set_clearing_type(ClearingType newType);

    [[nodiscard]] inline bool is_razed() const;
    inline void set_is_razed(bool newStatus);

    [[nodiscard]] std::vector<landmark_data::Landmark> get_landmarks() const;
    [[nodiscard]] inline bool is_landmark_present(landmark_data::Landmark desiredLandmark) const;
    std::expected<void, landmark_data::LandmarkError> set_landmarks(const std::vector<landmark_data::LandmarkStatusPair> &newLandmarkStatusPairs);
    std::expected<void, landmark_data::LandmarkError> set_landmarks(const std::vector<landmark_data::Landmark> &newLandmarks);
private:

    static constexpr uint8_t kMaxClearingConnections = 6;
    static constexpr uint8_t kConnectedClearingsCountBits = 3;
    static constexpr uint8_t kClearingConnectionTypeBits = 2;
    static constexpr uint8_t kClearingConnectionIndexBits = 4;
    static constexpr uint8_t kClearingConnectionBits = kClearingConnectionIndexBits + kClearingConnectionTypeBits;
    // Laid out as index, connectiontype, index, connectiontype, etc.
    static constexpr uint8_t kConnectedClearingsBits = kClearingConnectionBits * kMaxClearingConnections;

    static constexpr uint8_t kMaxForestConnections = 6;
    static constexpr uint8_t kConnectedForestsCountBits = 3;
    static constexpr uint8_t kForestConnectionIndexBits = 4;
    static constexpr uint8_t kConnectedForestsBits = kForestConnectionIndexBits * kMaxForestConnections;

    static constexpr uint8_t kMaxBuildingSlotCount = 4;
    static constexpr uint8_t kBuildingSlotCountBits = 3;
    static constexpr uint8_t kOccupiedBuildingSlotCountBits = kBuildingSlotCountBits;
    static constexpr uint8_t kBuildingSlotBits = 5;
    static constexpr uint8_t kBuildingSlotsBits = kBuildingSlotBits * kMaxBuildingSlotCount;

    static constexpr uint8_t kTreetopIndexBits = 3;

    struct TokenDataInfo
    {
        uint8_t offset;
        uint8_t width;
        uint8_t maxCount;
    };

    static constexpr std::array<TokenDataInfo, 21> kTokenDataInfoField = {{
        {0, 3, 8},  // Wood
        {3, 1, 1},  // Keep
        {4, 1, 1},  // Sympathy
        {5, 1, 1},  // Mouse Trade Post
        {6, 1, 1},  // Fox Trade Post
        {7, 1, 1},  // Rabbit Trade Post
        {8, 1, 1},  // Tunnel
        {9, 1, 1},  // Bomb Plot
        {10, 2, 2}, // Snare Plot
        {12, 2, 2}, // Extortion Plot
        {15, 2, 2}, // Raid Plot
        {17, 1, 1}, // Mob
        {18, 1, 1}, // Figure Value 1
        {19, 1, 1}, // Figure Value 2
        {20, 2, 2}, // Figure Value 3
        {22, 1, 1}, // Tablet Value 1
        {23, 1, 1}, // Tablet Value 2
        {24, 2, 2}, // Tablet Value 3
        {26, 1, 1}, // Jewelry Value 1
        {27, 1, 1}, // Jewelry Value 2
        {28, 2, 2}, // Jewelry Value 3
    }};

    static constexpr uint8_t kHiddenPlotToggleBits = 1;
    static constexpr uint8_t kTokenDataBits = kTokenDataInfoField.back().offset + kTokenDataInfoField.back().width;

    struct PawnDataInfo
    {
        uint8_t offset;
        uint8_t width;
        uint8_t maxCount;
    };

    static constexpr std::array<PawnDataInfo, 12> kPawnDataInfoField = {{
        {0, 5, 25}, // Marquise de Cat Pawns
        {5, 5, 20}, // Eyrie Dynasties Pawns
        {10, 1, 1}, // Vagabond 1
        {11, 1, 1}, // Vagabond 2
        {12, 4, 10}, // Woodland Alliance Pawns
        {16, 4, 15}, // Riverfolk Company Pawns
        {20, 5, 25}, // Lizard Cult Pawns
        {25, 4, 15}, // Corvid Conspiracy Pawns
        {29, 5, 20}, // Underground Duchy Pawns
        {34, 5, 20}, // Lord of The Hundred Pawns
        {39, 1, 1}, // Lord of the Hundred Warlord
        {40, 4, 15} // Keepers in Iron Pawns
    }};

    static constexpr uint8_t kPawnDataBits = kPawnDataInfoField.back().offset + kPawnDataInfoField.back().width;

    static constexpr uint8_t kClearingTypeBits = 2;
    static constexpr uint8_t kRazedBits = 1;
    static constexpr uint8_t kLandmarkBits = 5;

    static constexpr uint8_t kConnectedClearingsCountOffset = 0;
    static constexpr uint8_t kConnectedClearingsOffset = kConnectedClearingsCountOffset + kConnectedClearingsCountBits;
    static constexpr uint8_t kConnectedForestsCountOffset = kConnectedClearingsOffset + kConnectedClearingsBits;
    static constexpr uint8_t kConnectedForestsOffset = kConnectedForestsCountOffset + kConnectedForestsCountBits;
    static constexpr uint8_t kBuildingSlotCountOffset = kConnectedForestsOffset + kConnectedForestsBits;
    static constexpr uint8_t kOccupiedBuildingSlotCountOffset = kBuildingSlotCountOffset + kBuildingSlotCountBits;
    static constexpr uint8_t kBuildingSlotsOffset = kOccupiedBuildingSlotCountOffset + kOccupiedBuildingSlotCountBits;
    static constexpr uint8_t kTreetopIndexOffset = kBuildingSlotsOffset + kBuildingSlotsBits;
    static constexpr uint8_t kTokenDataOffset = kTreetopIndexOffset + kTreetopIndexBits;
    static constexpr uint8_t kHiddenPlotToggleOffset = kTokenDataOffset + kTokenDataBits;
    static constexpr uint8_t kPawnDataOffset = kHiddenPlotToggleOffset + kHiddenPlotToggleBits;
    static constexpr uint8_t kClearingTypeOffset = kPawnDataOffset + kPawnDataBits;
    static constexpr uint8_t kRazedOffset = kClearingTypeOffset + kClearingTypeBits;
    static constexpr uint8_t kLandMarkOffset = kRazedOffset + kRazedBits;

    std::array<uint8_t, 23> clearingData{};

    // Wrappers for read and write bits functions to allow for ease of use
    template <game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t shift, uint16_t width>
    [[nodiscard]] inline OutputType read_bits() const {
        return game_data::read_bits<OutputType, sizeof(clearingData), shift, width>(clearingData);
    }

    template <game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t width>
    [[nodiscard]] inline OutputType read_bits(uint16_t shift) const {
        return game_data::read_bits<OutputType, sizeof(clearingData), width>(clearingData, shift);
    }

    template <game_data::IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
    [[nodiscard]] inline OutputType read_bits() const {
        return game_data::read_bits<OutputType, sizeof(clearingData), shift, elementWidth>(clearingData);
    }

    template <game_data::IsValidByteArray OutputType, uint8_t elementWidth>
    [[nodiscard]] inline OutputType read_bits(uint16_t shift) const {
        return game_data::read_bits<OutputType, sizeof(clearingData), elementWidth>(clearingData, shift);
    }

    template <game_data::IsUnsignedIntegralOrEnum InputType, uint16_t shift, uint16_t width>
    inline void write_bits(const InputType &value) {
        game_data::write_bits<InputType, sizeof(clearingData), shift, width>(clearingData, value);
    }

    template <game_data::IsUnsignedIntegralOrEnum InputType, uint16_t width>
    inline void write_bits(const InputType &value, uint16_t shift) {
        game_data::write_bits<InputType, sizeof(clearingData), width>(clearingData, value, shift);
    }

    template <game_data::IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
    inline void write_bits(const InputType &value) {
        game_data::write_bits<InputType, sizeof(clearingData), shift, elementWidth>(clearingData, value);
    }

    template <game_data::IsValidByteArray InputType, uint8_t elementWidth>
    inline void write_bits(const InputType &value, uint16_t shift) {
        game_data::write_bits<InputType, sizeof(clearingData), elementWidth>(clearingData, value, shift);
    }

    [[nodiscard]] inline std::expected<uint8_t, ConnectionError> get_forest_connection_count() {
        return board_data::get_basic_connection_count<kConnectedForestsCountBits, kConnectedForestsCountOffset, kMaxForestConnections, sizeof(clearingData)>(clearingData);
    }

    template <uint8_t newCount>
    inline void set_forest_connection_count() {
        return board_data::set_basic_connection_count<kConnectedForestsCountBits, kConnectedForestsCountOffset, kMaxForestConnections, sizeof(clearingData), newCount>(clearingData);
    }

    [[nodiscard]] inline std::expected<void, ConnectionError> set_forest_connection_count(uint8_t newCount) {
        return board_data::set_basic_connection_count<kConnectedForestsCountBits, kConnectedForestsCountOffset, kMaxForestConnections, sizeof(clearingData)>(clearingData, newCount);
    }

    [[nodiscard]] inline std::expected<std::vector<uint8_t>, ConnectionError> get_forest_connections() {
        return board_data::get_basic_connections<kConnectedForestsCountBits, kConnectedForestsCountOffset, kMaxForestConnections, kForestConnectionIndexBits, kConnectedForestsOffset, sizeof(clearingData)>(clearingData);
    }

    [[nodiscard]] inline std::expected<void, ConnectionError> set_forest_connections(const std::vector<uint8_t> &newConnections) {
        return board_data::set_basic_connections<kConnectedForestsCountBits, kConnectedForestsCountOffset, kMaxForestConnections, kForestConnectionIndexBits, kConnectedForestsOffset, sizeof(clearingData)>(clearingData, newConnections);
    }

    [[nodiscard]] inline std::expected<void, ConnectionError> add_forest_connections(const std::vector<uint8_t> &newConnections) {
        return board_data::add_basic_connections<kConnectedForestsCountBits, kConnectedForestsCountOffset, kMaxForestConnections, kForestConnectionIndexBits, kConnectedForestsOffset, sizeof(clearingData)>(clearingData, newConnections);
    }

    [[nodiscard]] std::expected<void, ConnectionError> remove_forest_connections(const std::vector<uint8_t> &indices) {
        return board_data::remove_basic_connections<kConnectedForestsCountBits, kConnectedForestsCountOffset, kMaxForestConnections, kForestConnectionIndexBits, kConnectedForestsOffset, sizeof(clearingData)>(clearingData, indices);
    }

    template <size_t I, size_t N>
    inline static void unroll_separate_connections(const std::array<uint8_t, N> &combinedArray, std::vector< ClearingConnection> &separatedVector);

    template <size_t I, size_t N>
    inline static void unroll_separate_connections_filtered(const std::array<uint8_t, N> &combinedArray, std::vector<uint8_t> &separatedVector, ClearingConnectionType desiredConnectionType);

    template <size_t I, size_t N>
    inline void unroll_combine_clearing_connections(const std::vector<ClearingConnection> &separatedVector, std::array<uint8_t, N> &combinedArray);

    template<game_data::faction_data::FactionID factionID>
    inline std::expected<void, PawnError> set_pawn_count_generic(uint8_t newCount);

    [[nodiscard]] inline uint8_t get_occupied_slot_count_unsafe() const;
};
} // clearing_data
} // board_data
} // game_data