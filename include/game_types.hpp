#ifndef GAME_TYPES_HPP
#define GAME_TYPES_HPP

#include <cstdint>
#include <array>

static constexpr auto TOTAL_CLEARINGS = 12;
static constexpr auto TOTAL_FORESTS = 12;

enum class ClearingConnection : uint8_t
{
    NoConnection,
    Normal,
    ByWater,
    Blocked,
    MaxConnectionIndex // for bounds checking
};

enum class Token : uint8_t
{
    Wood,
    Keep,
    Sympathy,
    MouseTradePost,
    FoxTradePost,
    RabbitTradePost,
    Tunnel,
    BombPlot,
    SnarePlot,
    ExtortionPlot,
    RaidPlot,
    Mob,
    FigureValue1,
    FigureValue2,
    FigureValue3,
    TabletValue1,
    TabletValue2,
    TabletValue3,
    JewelryValue1,
    JewelryValue2,
    JewelryValue3,
    MaxTokenIndex // for bounds checking
};

struct TokenInfo
{
    uint8_t offset;
    uint8_t width;
};

static constexpr TokenInfo TOKEN_FIELD_INFO[] = {
    {0, 3},  // Wood
    {3, 1},  // Keep
    {4, 1},  // Sympathy
    {5, 1},  // Mouse Trade Post
    {6, 1},  // Fox Trade Post
    {7, 1},  // Rabbit Trade Post
    {8, 1},  // Tunnel
    {9, 1},  // Bomb Plot
    {10, 2}, // Snare Plot
    {12, 2}, // Extortion Plot
    {15, 2}, // Raid Plot
    {17, 1}, // Mob
    {18, 1}, // Figure Value 1
    {19, 1}, // Figure Value 2
    {20, 2}, // Figure Value 3
    {22, 1}, // Tablet Value 1
    {23, 1}, // Tablet Value 2
    {24, 2}, // Tablet Value 3
    {26, 1}, // Jewelry Value 1
    {27, 1}, // Jewelry Value 2
    {28, 2}, // Jewelry Value 3
};

enum class Building : uint8_t
{
    NoBuilding,
    Ruin,
    Workshop,
    Sawmill,
    Recruiter,
    Roost,
    MouseBase,
    FoxBase,
    RabbitBase,
    MouseGarden,
    FoxGarden,
    RabbitGarden,
    Citadel,
    Market,
    Stronghold,
    FigureAndTabletWaystation,
    TabletAndJewelryWaystation,
    JewelryAndFigureWaystation,
    NotASlot,
    MaxBuildingIndex // for bounds checking
};

struct BuildingSlot
{
    Building currentBuilding;
    bool isOnElderTreetop;
};

//Ordered in Faction Setup Order
enum class FactionID : uint8_t
{
    MarquiseDeCat, // A
    EyrieDynasties, // B
    WoodlandAlliance, // C
    Vagabond1, // D
    Vagabond2, // E
    LizardCult, // F
    RiverfolkCompany, // G
    UndergroundDuchy, // H
    CorvidConspiracy, // I
    LordOfTheHundreds, // J
    KeepersInIron, // K
    MaxFactionID
};

struct PawnInfo
{
    uint8_t offset;
    uint8_t width;
};

static constexpr PawnInfo PAWN_FIELD_INFO[] = {
    {0, 5},  // Marquise de Cat Pawns
    {5, 5},  // Eyrie Dynasties Pawns
    {10, 4}, // Woodland Alliance Pawns
    {14, 1}, // Vagabond 1
    {15, 1}, // Vagabond 2
    {16, 5}, // Lizard Cult Pawns
    {21, 4}, // Riverfolk Company Pawns
    {25, 4}, // Corvid Conspiracy Pawns
    {29, 5}, // Underground Duchy Pawns
    {34, 5}, // Lord of the Hundreds Pawns
    {39, 1}, // Lord of the Hundreds Warlord
    {40, 4}, // Keepers in Iron Pawns

};

enum class ClearingType : uint8_t
{
    None,
    Mouse,
    Fox,
    Rabbit,
    MaxClearingTypeIndex // for bounds checking
};

enum class Landmark : uint8_t
{
    None,
    Ferry,
    Tower,
    LostCity,
    LegendaryForge,
    BlackMarket,
    LandmarkCount // for bounds checking
};

enum class Map : uint8_t
{
    Autumn,
    Winter,
    Lake,
    Mountain,
    MapCount,
    Random
};

enum class Item : uint8_t
{
    Bag,
    Boots,
    Hammer,
    Sword,
    Tea,
    Coin,
    Crossbow,
    Torch
};

enum class SetupType : uint8_t
{
    Standard,
    Advanced
};

enum class DeckType : uint8_t
{
    Standard,
    ExilesAndPartisans
};

enum class CardID : uint8_t
{
    MouseDominance,
    FoxDominance,
    RabbitDominance,
    BirdDominance,
    MouseAmbush,
    FoxAmbush,
    RabbitAmbush,
    BirdAmbush,
    MouseInASack,
    GentlyUsedKnapsack,
    SmugglersTrail,
    BirdyBindle,
    MouseTravelGear,
    FoxTravelGear,
    AVisitToFriends,
    WoodlandRunners,
    Anvil,
    Sword,
    FoxfolkSteel,
    ArmsTrader,
    MouseRootTea,
    FoxRootTea,
    RabbitRootTea,
    Investments,
    ProtectionRacket,
    BakeSale,
    MouseCrossbow,
    BirdCrossbow,
    FavorOfTheMice,
    FavorOfTheFoxes,
    FavorOfTheRabbits,
    Codebreakers,
    ScoutingParty,
    StandAndDeliver,
    TaxCollector,
    BetterBurrowBank,
    Cobbler,
    CommandWarren,
    Armorers,
    BrutalTactics,
    RoyalClaim,
    Sappers,
    MousePartisans,
    FoxPartisans,
    RabbitPartisans,
    LeagueOfAdventurousMice,
    MasterEngravers,
    MurineBroker,
    CharmOffensive,
    CoffinMakers,
    SwapMeet,
    Tunnels,
    FalseOrders,
    Informants,
    PropagandaBureau,
    BoatBuilders,
    CorvidPlanners,
    EyrieÉmigré,
    Saboteurs,
    SoupKitchens
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

    24 bits: Building Slots (4 slots times 5 bits for building type, plus 1 bit for Elder Treetop Toggle) {
        Slot (6 bits): {
            Current Building (5 bits): {
                0 = Not a slot in this clearing
                1 = No building,
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

    12 Bits: Connected Forests {
        1 Bit: Is connected? (0-1)
    } x 12 for each forest

    1 Bit: Razed? (0-1)

    3 Bits: Landmarks {
        0 = None

    }
    */
public:
    Clearing() : data1(0), data2(0), data3(0) {}

    template <typename T = uint8_t>
    T read_bits(uint8_t shift, uint8_t width) const;

    void write_bits(uint64_t value, uint8_t shift, uint8_t width);

    ClearingConnection get_clearing_connection(uint8_t index) const;
    bool set_clearing_connection(uint8_t index, ClearingConnection value);

    BuildingSlot get_building_slot(uint8_t index) const;
    bool set_building_slot(uint8_t index, const BuildingSlot &slot);

    uint8_t get_token_count(Token token) const;
    bool set_token_count(Token token, uint8_t count);

    uint8_t get_pawn_count(FactionID factionID) const;
    bool set_pawn_count(FactionID factionID, uint8_t count);

    bool is_lord_of_the_hundreds_warlord_present() const;
    void set_lord_of_the_hundreds_warlord_present(bool present);

    ClearingType get_clearing_type() const;
    void set_clearing_type(ClearingType type);

    bool get_connected_forest(uint8_t index) const;
    void set_connected_forest(uint8_t index, bool value);

    bool is_razed() const;
    void set_razed(bool value);

    Landmark get_landmark() const;
    void set_landmark(Landmark landmark);

    uint8_t get_next_empty_building_slot_index(uint8_t starting_index) const;

private:
    uint64_t data1 = 0;
    uint64_t data2 = 0;
    uint32_t data3 = 0;

    static constexpr auto CLEARING_CONNECTION_BITS = 2;
    static constexpr auto BUILDING_TYPE_BITS = 5;
    static constexpr auto TREETOP_TOGGLE_BITS = 1;
    static constexpr auto BUILDING_SLOT_BITS = BUILDING_TYPE_BITS + TREETOP_TOGGLE_BITS;
    static constexpr auto FOREST_CONNECTION_BITS = 1;

    static constexpr auto TOTAL_BUILDING_SLOTS = 4;

    static constexpr auto CONNECTED_CLEARINGS_BITS = CLEARING_CONNECTION_BITS * TOTAL_CLEARINGS;
    static constexpr auto BUILDING_SLOTS_BITS = BUILDING_SLOT_BITS * TOTAL_BUILDING_SLOTS;
    static constexpr auto TOKENS_BITS = 28;
    static constexpr auto PAWN_DATA_BITS = 44;
    static constexpr auto CLEARING_TYPE_BITS = 2;
    static constexpr auto CONNECTED_FORESTS_BITS = FOREST_CONNECTION_BITS * TOTAL_FORESTS;
    static constexpr auto RAZED_BITS = 1;
    static constexpr auto LANDMARK_BITS = 3;

    static constexpr auto CONNECTED_CLEARINGS_SHIFT = 0;
    static constexpr auto BUILDING_SLOTS_SHIFT = CONNECTED_CLEARINGS_SHIFT + CONNECTED_CLEARINGS_BITS;
    static constexpr auto TOKENS_SHIFT = BUILDING_SLOTS_SHIFT + BUILDING_SLOTS_BITS;
    static constexpr auto PAWN_DATA_SHIFT = TOKENS_SHIFT + TOKENS_BITS;
    static constexpr auto CLEARING_TYPE_SHIFT = PAWN_DATA_SHIFT + PAWN_DATA_BITS;
    static constexpr auto CONNECTED_FORESTS_SHIFT = CLEARING_TYPE_SHIFT + CLEARING_TYPE_BITS;
    static constexpr auto RAZED_SHIFT = CONNECTED_FORESTS_SHIFT + CONNECTED_FORESTS_BITS;
    static constexpr auto LANDMARK_SHIFT = RAZED_SHIFT + RAZED_BITS;

    static constexpr auto BUILDING_TYPE_MASK = (1 << BUILDING_TYPE_BITS) - 1;
    static constexpr auto TREETOP_TOGGLE_MASK = (1 << TREETOP_TOGGLE_BITS) - 1;
};

class Forest
{
    /*
    12 Bits: Relics {
        Bit 1: Figure Value 1 (0-1)
        Bit 2: Figure Value 2 (0-1)
        Bits 3-4: Figure Value 3 (0-2, 3 = unused)
        Bit 5: Tablet Value 1 (0-1)
        Bit 6: Tablet Value 2 (0-1)
        Bits 7-8: Tablet Value 3 (0-2, 3 = unused)
        Bit 9: Jewelry Value 1 (0-1)
        Bit 10: Jewelry Value 2 (0-1)
        Bits 11-12: Jewelry Value 3 (0-2, 3 = unused)
    }

    2 Bits: Vagabonds {
        Bit 1: Vagabond 1 (0-1)
        Bit 2: Vagabond 2 (0-1)
    }

    12 Bits: Connected Clearings {
        1 Bit: Is connected? (0-1)
    } x 12 for each clearing
    */

public:
    Forest() : data(0) {}

    template <typename T = uint8_t>
    T read_bits(uint8_t shift, uint8_t width) const;

    void write_bits(uint32_t value, uint8_t shift, uint8_t width);

    std::array<Token, (static_cast<uint8_t>(Token::JewelryValue3) - static_cast<uint8_t>(Token::FigureValue1) + 1)> get_relics() const;
    bool set_relic(Token token, uint8_t count);

    bool get_vagabond(uint8_t index) const;
    bool set_vagabond(uint8_t index, bool value);

    bool get_connected_clearing(uint8_t index) const;
    bool set_connected_clearing(uint8_t index, bool value);

private:
    uint32_t data;

    static constexpr auto RELIC_BITS = 12;

    static constexpr auto VAGABOND_BITS = 1;
    static constexpr auto TOTAL_VAGABONDS = 2;
    static constexpr auto VAGABONDS_BITS = VAGABOND_BITS * TOTAL_VAGABONDS;

    static constexpr auto CLEARING_CONNECTION_BITS = 1;
    static constexpr auto CONNECTED_CLEARINGS_BITS = CLEARING_CONNECTION_BITS * TOTAL_CLEARINGS;

    static constexpr auto RELICS_SHIFT = 0;
    static constexpr auto VAGABONDS_SHIFT = RELICS_SHIFT + RELIC_BITS;
    static constexpr auto CONNECTED_CLEARINGS_SHIFT = VAGABONDS_SHIFT + VAGABONDS_BITS;
};

#endif // GAME_TYPES_HPP