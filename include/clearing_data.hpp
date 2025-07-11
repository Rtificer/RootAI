#pragma once

#include "factions_data.hpp"
#include "token_data.hpp"
#include "board_data.hpp"

#include <cstdint>
#include <array>

namespace game_data 
{
namespace board_data 
{
namespace clearing_data
{

namespace building_data
{
enum class Building : uint8_t
{
    kNoBuilding,
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
    kNotASlot,
    kMaxBuildingIndex // for bounds checking
};

struct BuildingSlot
{
    Building currentBuilding;
    bool isOnElderTreetop;
};
}

namespace landmark_data
{
enum class Landmark : uint8_t
{
    kNone,
    kFerry,
    kTower,
    kLostCity,
    kLegendaryForge,
    kBlackMarket,
    kLandmarkCount // for bounds checking
};
}

enum class ClearingConnection : uint8_t
{
    kNoConnection,
    kNormal,
    kByWater,
    kBlocked,
    kMaxConnectionIndex // for bounds checking
};

enum class ClearingType : uint8_t
{
    kNone,
    kMouse,
    kFox,
    kRabbit,
    kMaxClearingTypeIndex // for bounds checking
};

class Clearing
{
    /*
    24 bits: Connected Clearings {
    Connection (2 bits): {
    0 = No connection
    1 = Normal
    2 = By water
    3 = Blocked
    } x 12 for each clearing
    }

    24 bits: Building Slots (4 slots times 5 bits for building type, plus 1 bit
    for Elder Treetop Toggle) { Slot (6 bits): { Current Building (5 bits): { 0 =
    Not a slot in this clearing 1 = No building, 2 = Ruin, 3 = Workshop, 4 =
    Sawmill, 5 = Roost, 6 = Mouse Base, 7 = Fox Base, 8 = Rabbit Base, 9 = Mouse
    Garden, 10 = Fox Garden, 11 = Rabbit Garden, 12 = Citadel, 13 = Market, 14 =
    Stronghold, 15 = Figure / Tablet Waystation 16 = Tablet / Jewelry Waystation
    17 = Jewelry / Figure Waystation
    18-31 = unused
    }
    Is on Elder Treetop? (1 bit): {
    0 = No
    1 = Yes
    }
    } x 4 for each possible slot
    }

    28 Bits: Tokens {
    Bits 1-3: Wood (0-8 (9-15 unused))
    Bit 4: Keep (0-1)
    Bit 5: Sympathy (0-1)
    Bit 6: Mouse Trade Post (0-1)
    Bit 7: Fox Trade Post (0-1)
    Bit 8: Rabbit Trade Post (0-1)
    Bit 9: Bomb Plot (0-1)
    Bit 10-11: Snare Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
    Bits 12-13: Extortion Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 =
    unused) Bits 14-15: Raid Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 =
    unused) Bit 16: Tunnel (0-1) Bit 17: Mob (0-1) Bit 18 Figure Value 1 (0-1) Bit
    19 Figure Value 2 (0-1) Bits 20-21 Figure Value 3 (0-2, 3 = unused) Bit 22:
    Tablet Value 1 (0-1) Bit 23: Tablet Value 2 (0-1) Bits 24-25: Tablet Value 3
    (0-2, 3 = unused) Bit 26: Jewelry Value 1 (0-1) Bit 27: Jewelry Value 2 (0-1)
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

    12 Bits: Connected Forests {
    1 Bit: Is connected? (0-1)
    } x 12 for each forest

    1 Bit: Razed? (0-1)

    3 Bits: Landmarks {
    0 = None

    }
    */
public:
    Clearing() : data{} {}

    template <typename T = uint8_t>
    [[nodiscard]] T read_bits(uint8_t shift, uint8_t width) const;

    void write_bits(uint8_t value, uint8_t shift, uint8_t width);

    [[nodiscard]] ClearingConnection get_clearing_connection(uint8_t index) const;
    bool set_clearing_connection(uint8_t index, ClearingConnection value);

    [[nodiscard]] building_data::BuildingSlot get_building_slot(uint8_t index) const;
    bool set_building_slot(uint8_t index, const building_data::BuildingSlot &slot);

    [[nodiscard]] uint8_t get_token_count(token_data::Token token) const;
    bool set_token_count(token_data::Token token, uint8_t count);

    [[nodiscard]] uint8_t get_pawn_count(::game_data::faction_data::FactionID factionID) const;
    bool set_pawn_count(::game_data::faction_data::FactionID factionID, uint8_t count);

    [[nodiscard]] bool is_lord_of_the_hundreds_warlord_present() const;
    void set_lord_of_the_hundreds_warlord_present(bool present);

    [[nodiscard]] ClearingType get_clearing_type() const;
    void set_clearing_type(ClearingType type);

    [[nodiscard]] bool get_connected_forest(uint8_t index) const;
    void set_connected_forest(uint8_t index, bool value);

    [[nodiscard]] inline bool is_razed() const;
    inline void set_razed(bool value);

    [[nodiscard]] inline landmark_data::Landmark get_landmark() const;
    inline void set_landmark(landmark_data::Landmark landmark);

    [[nodiscard]] uint8_t get_next_empty_building_slot_index(
        uint8_t starting_index) const;

private:
    std::array<uint8_t, 18>
        data; // 128 bits total: 64 + 64 + 32 = 160 bits rounded up to 16 bytes

    static constexpr uint8_t kClearingConnectionBits = 2;
    static constexpr uint8_t kBuildingTypeBits = 5;
    static constexpr uint8_t kTreetopToggleBits = 1;
    static constexpr uint8_t kBuildingSlotBits =
        kBuildingTypeBits + kTreetopToggleBits;
    static constexpr uint8_t kForestConnectionBits = 1;

    static constexpr uint8_t kTotalBuildingSlots = 4;

    static constexpr uint8_t kConnectedClearingsBits =
        kClearingConnectionBits * kTotalClearings;
    static constexpr uint8_t kBuildingSlotsBits =
        kBuildingSlotBits * kTotalBuildingSlots;
    static constexpr uint8_t kTokensBits = 28;
    static constexpr uint8_t kPawnDataBits = 44;
    static constexpr uint8_t kClearingTypeBits = 2;
    static constexpr uint8_t kConnectedForestsBits =
        kForestConnectionBits * game_data::board_data::kTotalForests;
    static constexpr uint8_t kRazedBits = 1;
    static constexpr uint8_t kLandmarkBits = 3;

    static constexpr uint8_t kConnectedClearingsShift = 0;
    static constexpr uint8_t kBuildingSlotsShift =
        kConnectedClearingsShift + kConnectedClearingsBits;
    static constexpr uint8_t kTokensShift = kBuildingSlotsShift + kBuildingSlotsBits;
    static constexpr uint8_t kPawnDataShift = kTokensShift + kTokensBits;
    static constexpr uint8_t kClearingTypeShift = kPawnDataShift + kPawnDataBits;
    static constexpr uint8_t kConnectedForestsShift =
        kClearingTypeShift + kClearingTypeBits;
    static constexpr uint8_t kRazedShift =
        kConnectedForestsShift + kConnectedForestsBits;
    static constexpr uint8_t kLandmarkShift = kRazedShift + kRazedBits;

    static constexpr uint8_t kBuildingTypeMask = (1 << kBuildingTypeBits) - 1;
    static constexpr uint8_t kTreetopToggleMask = (1 << kTreetopToggleBits) - 1;
};
}
}
}