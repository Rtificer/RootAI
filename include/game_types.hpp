#pragma once

#include <array>
#include <cstdint>
#include <vector>

static constexpr auto kTotalClearings = 12;
static constexpr auto kTotalForests = 12;

enum class ClearingConnection : uint8_t
{
    kNoConnection,
    kNormal,
    kByWater,
    kBlocked,
    kMaxConnectionIndex // for bounds checking
};

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

enum class ClearingType : uint8_t
{
    kNone,
    kMouse,
    kFox,
    kRabbit,
    kMaxClearingTypeIndex // for bounds checking
};

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

    [[nodiscard]] BuildingSlot get_building_slot(uint8_t index) const;
    bool set_building_slot(uint8_t index, const BuildingSlot &slot);

    [[nodiscard]] uint8_t get_token_count(Token token) const;
    bool set_token_count(Token token, uint8_t count);

    [[nodiscard]] uint8_t get_pawn_count(FactionID factionID) const;
    bool set_pawn_count(FactionID factionID, uint8_t count);

    [[nodiscard]] bool is_lord_of_the_hundreds_warlord_present() const;
    void set_lord_of_the_hundreds_warlord_present(bool present);

    [[nodiscard]] ClearingType get_clearing_type() const;
    void set_clearing_type(ClearingType type);

    [[nodiscard]] bool get_connected_forest(uint8_t index) const;
    void set_connected_forest(uint8_t index, bool value);

    [[nodiscard]] bool is_razed() const;
    void set_razed(bool value);

    [[nodiscard]] Landmark get_landmark() const;
    void set_landmark(Landmark landmark);

    [[nodiscard]] uint8_t get_next_empty_building_slot_index(
        uint8_t starting_index) const;

private:
    std::array<uint8_t, 18>
        data; // 128 bits total: 64 + 64 + 32 = 160 bits rounded up to 16 bytes

    static constexpr auto kClearingConnectionBits = 2;
    static constexpr auto kBuildingTypeBits = 5;
    static constexpr auto kTreetopToggleBits = 1;
    static constexpr auto kBuildingSlotBits =
        kBuildingTypeBits + kTreetopToggleBits;
    static constexpr auto kForestConnectionBits = 1;

    static constexpr auto kTotalBuildingSlots = 4;

    static constexpr auto kConnectedClearingsBits =
        kClearingConnectionBits * kTotalClearings;
    static constexpr auto kBuildingSlotsBits =
        kBuildingSlotBits * kTotalBuildingSlots;
    static constexpr auto kTokensBits = 28;
    static constexpr auto kPawnDataBits = 44;
    static constexpr auto kClearingTypeBits = 2;
    static constexpr auto kConnectedForestsBits =
        kForestConnectionBits * kTotalForests;
    static constexpr auto kRazedBits = 1;
    static constexpr auto kLandmarkBits = 3;

    static constexpr auto kConnectedClearingsShift = 0;
    static constexpr auto kBuildingSlotsShift =
        kConnectedClearingsShift + kConnectedClearingsBits;
    static constexpr auto kTokensShift = kBuildingSlotsShift + kBuildingSlotsBits;
    static constexpr auto kPawnDataShift = kTokensShift + kTokensBits;
    static constexpr auto kClearingTypeShift = kPawnDataShift + kPawnDataBits;
    static constexpr auto kConnectedForestsShift =
        kClearingTypeShift + kClearingTypeBits;
    static constexpr auto kRazedShift =
        kConnectedForestsShift + kConnectedForestsBits;
    static constexpr auto kLandmarkShift = kRazedShift + kRazedBits;

    static constexpr auto kBuildingTypeMask = (1 << kBuildingTypeBits) - 1;
    static constexpr auto kTreetopToggleMask = (1 << kTreetopToggleBits) - 1;
};

enum class Token : uint8_t
{
    kWood,
    kKeep,
    kSympathy,
    kMouseTradePost,
    kFoxTradePost,
    kRabbitTradePost,
    kTunnel,
    kBombPlot,
    kSnarePlot,
    kExtortionPlot,
    kRaidPlot,
    kMob,
    kFigureValue1,
    kFigureValue2,
    kFigureValue3,
    kTabletValue1,
    kTabletValue2,
    kTabletValue3,
    kJewelryValue1,
    kJewelryValue2,
    kJewelryValue3,
    kMaxTokenIndex // for bounds checking
};

struct TokenInfo
{
    uint8_t offset;
    uint8_t width;
};

static constexpr TokenInfo kTokenFieldInfo[] = {
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

// Ordered in Faction Setup Order

struct PawnInfo
{
    uint8_t offset;
    uint8_t width;
};

static constexpr PawnInfo kPawnFieldInfo[] = {
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

enum class Map : uint8_t
{
    kAutumn,
    kWinter,
    kLake,
    kMountain,
    kMapCount,
    kRandom
};

enum class Item : uint8_t
{
    kBag,
    kBoots,
    kHammer,
    kSword,
    kTea,
    kCoin,
    kCrossbow,
    kTorch
};

enum class SetupType : uint8_t
{
    kStandard,
    kAdvanced
};

enum class DeckType : uint8_t
{
    kStandard,
    kExilesAndPartisans
};

namespace card_data
{
    static constexpr uint8_t kCardIDBits = 6;

    enum class CardID : uint8_t
    {
        // Dominance
        kMouseDominance,
        kFoxDominance,
        kRabbitDominance,
        kBirdDominance,
        
        // Ambush
        kMouseAmbush,
        kFoxAmbush,
        kRabbitAmbush,
        kBirdAmbush,

        // Items
        kMouseInASack,
        kGentlyUsedKnapsack,
        kSmugglersTrail,
        kBirdyBindle,
        kMouseTravelGear,
        kFoxTravelGear,
        kAVisitToFriends,
        kWoodlandRunners,
        kAnvil,
        kSword,
        kFoxfolkSteel,
        kArmsTrader,
        kMouseRootTea,
        kFoxRootTea,
        kRabbitRootTea,
        kInvestments,
        kProtectionRacket,
        kBakeSale,
        kMouseCrossbow,
        kBirdCrossbow,

        // Standard Deck Specific
        kFavorOfTheMice,
        kFavorOfTheFoxes,
        kFavorOfTheRabbits,
        kCodebreakers,
        kScoutingParty,
        kStandAndDeliver,
        kTaxCollector,
        kBetterBurrowBank,
        kCobbler,
        kCommandWarren,
        kArmorers,
        kBrutalTactics,
        kRoyalClaim,
        kSappers,
        kMousePartisans,
        kFoxPartisans,
        kRabbitPartisans,

        // Exiles and Partisans Specific
        kLeagueOfAdventurousMice,
        kMasterEngravers,
        kMurineBroker,
        kCharmOffensive,
        kCoffinMakers,
        kSwapMeet,
        kTunnels,
        kFalseOrders,
        kInformants,
        kPropagandaBureau,
        kBoatBuilders,
        kCorvidPlanners,
        kEyrieÉmigré,
        kSaboteurs,
        kSoupKitchens,

        // Faction Specific
        kLoyalVizier,
        kFaithfulRetainer,
        kNotACard
    };
}

enum class Suit : uint8_t
{
    kMouse,
    kFox,
    kRabbit,
    kBird,
    kSuitCount
};

namespace vagabond
{
    enum class FactionRelationship : uint8_t
    {
        kHostile,
        kIndifferent,
        kRank1,
        kRank2,
        kAllied
    };
    enum class QuestCardID : uint8_t
    {
        kEscort,
        kMouseExpelBandits,
        kMouseFendOffABear,
        kMouseGuardDuty,
        kLogisticsHelp,
        kFoxErrand,
        kFundraising,
        kFoxGiveASpeech,
        kLogisticsHelp,
        kRepairAShed,
        kRabbitErrand,
        kRabbitExpelBandits,
        kRabbitFendOffABear,
        kRabbitGiveASpeech,
        kMouseGuardDuty
    };
}

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
    [[nodiscard]] T read_bits(uint8_t shift, uint8_t width) const;

    void write_bits(uint32_t value, uint8_t shift, uint8_t width);

    [[nodiscard]] std::array<Token,
                             (static_cast<uint8_t>(Token::kJewelryValue3) -
                              static_cast<uint8_t>(Token::kFigureValue1) + 1)>
    get_relics() const;
    bool set_relic(Token token, uint8_t count);

    [[nodiscard]] bool get_vagabond(uint8_t index) const;
    bool set_vagabond(uint8_t index, bool value);

    [[nodiscard]] bool get_connected_clearing(uint8_t index) const;
    bool set_connected_clearing(uint8_t index, bool value);

private:
    uint32_t data;

    static constexpr auto kRelicBits = 12;

    static constexpr auto kVagabondBits = 1;
    static constexpr auto kTotalVagabonds = 2;
    static constexpr auto kVagabondsBits = kVagabondBits * kTotalVagabonds;

    static constexpr auto kClearingConnectionBits = 1;
    static constexpr auto kConnectedClearingsBits =
        kClearingConnectionBits * kTotalClearings;

    static constexpr auto kRelicsShift = 0;
    static constexpr auto kVagabondsShift = kRelicsShift + kRelicBits;
    static constexpr auto kConnectedClearingsShift =
        kVagabondsShift + kVagabondsBits;
};

class FactionData
{
    /*
    Score (6 Bits)
    60= Mouse Dominance
    61 = Fox Dominance
    62 = Rabbit Dominance
    63 = Bird Dominance

    Hand (113 Bits):
        6 Bits Per Card times 18 Cards
        5 Bits for Hand Size




    Marquise De Cat (20 Bits):
        5 Bits: Pawns (0-25, 26-31 uneeded)
        2 Bits: Is allianced (0 = No, 1 = Vag 1, 2 = Vag2)
        4 Bits: Wood Supply (0-8 (9-15 unused))
        3 Bits: Remaining Sawmill Count (0-6, 7 unused)
        3 Bits: Remaining Workshop Count (0-6, 7 unused)
        3 Bits: Remaining Recruiter Count (0-6, 7 unused)

    Eyrie Dynasties (778 Bits):
        5 Bits: Pawns (0-20, 21-31 uneeded)
        2 Bits: Is allianced (0 = No, 1 = Vag 1, 2 = Vag2)
        3 Bits: Remaining Roost Count (0-7)
        Decree (768 Bits):
        6 Bits Per Card times 32 Cards times 4 Slots

    Woodland Alliance (337 Bits):
        4 Bits: Pawns (0-10, 11-15 uneeded)
        2 Bits: Is allianced (0 = No, 1 = Vag 1, 2 = Vag2)
        1 Bit: Mouse Base Remaining? (0-1)
        1 Bit: Fox Base Remaining? (0-1)
        1 Bit: Rabbit Base Remaining? (0-1)
        4 Bits: Remaining Sympathy Tokens (0-10, 11-15 unused)
        Supporters (324 bits):
        6 Bits Per Card times 54 cards.

    Vagabond 1 (87 Bits):
        2 Bits: Bag Count
        3 Bits: Boots Count
        2 Bits: Hammer Count
        3 Bits: Sword Count
        2 Bits: Tea Count
        2 Bits: Coin Count
        2 Bits: Crossbow Count
        1 Bits: Torch Count

    Quests (60 Bits):
        4 Bits Per Card times 15 Cards

    Faction Relations (10 Bits):
        2 Bits Per Faction, times 5 other factions

    Vagabond 2 (27 Bits):
        2 Bits: Bag Count
        3 Bits: Boots Count
        2 Bits: Hammer Count
        3 Bits: Sword Count
        2 Bits: Tea Count
        2 Bits: Coin Count
        2 Bits: Crossbow Count
        1 Bits: Torch Count

    Faction Relations (10 Bits):
        2 Bits Per Player, times 5 each other player

    Lizard Cult (348 Bits):
        5 Bits: Pawns (0-25, 26-31 unused)
        2 Bits: Is allianced (0 = No, 1 = Vag 1, 2 = Vag2)
        2 Bits: Is allianced (0 = No, 1 = Vag 1, 2 = Vag2)
        5 Bits: Acolytes(0-25, 26-31 unused)

        3 Bits: Remaining Mouse Gardens (0-5, 6-7 unused)
        3 Bits: Remaining Fox Gardens (0-5, 6-7 unused)
        3 Bits: Remaining Rabbit Gardens (0-5, 6-7 unused)

        2 Bits: Outcast Suit (0 = Mouse, 1 = Fox, 2 = Rabbit, 3 = unused)
        1 Bit: Hated Outcast? (yes or no)

        Lost Souls (324 bits):
            6 Bits Per Card times 54 cards.

    Riverfolk Company (30 Bits):
        4 Bits: Pawns (0-15)

        2 Bits: Is allianced (0 = No, 1 = Vag 1, 2 = Vag2)

        2 Bits: Remaining Mouse Gardens (0-3)
        2 Bits: Remaining Fox Gardens (0-3)
        2 Bits: Remaining Rabbit Gardens (0-3)

        4 Bits: Payments (0-15)
        4 Bits: Funds (0-15)
        4 Bits: Commitments (0-15)

        2 Bits: Hand Card Cost (1-4)
        2 Bits: Riverboats Cost (1-4)
        2 Bits: Mercenaries Cost (1-4)

        Corvid Conspiracy (10 Bits):
        4 Bits: Pawns (0-15)

        2 Bits: Remaining Bomb Plots (0-2, 3 unused)
        2 Bits: Remaining Snare Plots (0-3, 3 unused)
        2 Bits: Remaining Extortion Plots (0-3, 3 unused)

    Underground Duchy (28 Bits):
        5 Bits: Pawns (0-20, 21-31 uneeded)
        2 Bits: Is allianced (0 = No, 1 = Vag 1, 2 = Vag2)

        2 Bits: Remaining Citadel Count (0-3)
        2 Bits: Remaining Market Count (0-3)
        2 Bits: Remaining Stronghold Count (0-3)

        2 Bits: Remaining Tunnel Count (0-3)

        9 Bits: Is Minister Present?
            Bit 1: Foremole
            Bit 2: Captain
            Bit 3: Marshal
            Bit 4: Brigadier
            Bit 5: Banker
            Bit 6: Mayor
            Bit 7: Duchess of Mud
            Bit 8: Baron of Dirt
            Bit 9: Earl of Stone

        4 Bits: Remaining Crown Count (0-9, 10-15 unused)

    Lord of the Hundreds (15 Bits):
        5 Bits: Pawns (0-20, 21-31 unused)
        1 Bit: Is Warlord Remaining? (0-1)

        3 Bits: Remaining Stronghold Count (0-6, 7 unused)

        3 Bits: Remaining Mob Count (0-5, 6-7 unused)

        3 Bits: Active Mood (0 = Bitter, 1 = Grandiose, 2 = Jubilant, 3 = Lavish,
        4 = Relentless, 5 = Rowdy, 6 = Stubborn, 7 = Wrathful)

    Keepers in Iron (202 Bits):
        5 Bits: Pawns (0-15)

        2 Bits: Is allianced (0 = No, 1 = Vag 1, 2 = Vag2)

        1 Bit: is Figure and Tablet Waystation remaining?
        1 Bit: is Tablet And Jewelry Waystation remaining?
        1 Bit: is Jewelry And Figure Waystation remaining?

        12 Bits: Remaining Relics:
            Bit 1: Figure Value 1 (0-1)
            Bit 2: Figure Value 2 (0-1)
            Bit 3-4: Figures of Value 3 (0-2, 3 = unused)
            Bit 5: Tablet Value 1 (0-1)
            Bit 6: Tablet Value 2 (0-1)
            Bit 7-8: Tablets of Value 3 (0-2, 3 = unused)
            Bit 9: Jewelry Value 1 (0-1)
            Bit 10: Jewelry Value 2 (0-1)
            Bit 11-12: Jewelry of Value 3 (0-2, 3 = unused)

        Retinue (180 Bits):
            Bits 1-60 Move:
                6 Bits Per Card times 10 Cards
            Bits 61-120 Battle + Delve:
                6 Bits Per Card times 10 Cards
            Bits 121-180 Move + Recover:
                6 Bits Per Card times 10 Cards
    */

public:
    FactionData() : data{} {}

    template <typename T>
    [[nodiscard]] T read_bits(uint16_t shift, uint8_t width) const;
    void write_bits(const std::vector<uint8_t> &value, uint16_t shift, uint16_t width);

    [[nodiscard]] FactionID get_faction_id() const;
    void set_faction_id(FactionID factionID);

    [[nodiscard]] std::vector<card_data::CardID> get_hand() const;
    bool set_hand(std::vector<card_data::CardID> newHand);
    [[nodiscard]] card_data::CardID get_card_in_hand() const;
    bool remove_card_from_hand(uint8_t cardIndex);
    bool add_card_to_hand(card_data::CardID card);
    bool discard_card_from_hand(uint8_t cardIndex);
    bool draw_cards(std::array<card_data::CardID, 54> &Deck, uint8_t count);

    [[nodiscard]] std::vector<bool> get_hand_reveal_status() const;
    bool set_hand_reveal_status(const std::vector<bool> &status);
    bool set_card_index_reveal_status(uint8_t cardIndex, bool isRevealed);

    [[nodiscard]] uint8_t get_remaining_pawn_count() const;
    bool set_remaining_pawn_count(uint8_t count);

    [[nodiscard]] uint8_t get_remaining_wood() const;
    bool set_remaining_wood(uint8_t count);
    [[nodiscard]] uint8_t get_remaining_sawmill_count() const;
    bool set_remaining_sawmill_count(uint8_t count);
    [[nodiscard]] uint8_t get_remaining_workshop_count() const;
    bool set_remaining_workshop_count(uint8_t count);
    [[nodiscard]] uint8_t get_remaining_recruiter_count() const;
    bool set_remaining_recruiter_count(uint8_t count);

    [[nodiscard]] uint8_t get_remaining_roost_count() const;
    bool set_remaining_roost_count(uint8_t count);
    [[nodiscard]] std::array<std::vector<card_data::CardID>, static_cast<size_t>(eyrie::DecreeStack::DecreeStackCount)> get_decree() const;
    bool set_decree(const std::array<std::vector<card_data::CardID>, static_cast<size_t>(eyrie::DecreeStack::DecreeStackCount)> &decree);
    bool append_card_to_decree(eyrie::DecreeStack stack, card_data::CardID card);
    bool setup_loyal_viziers(eyrie::Leader leader);
    [[nodiscard]] uint8_t get_decree_bird_card_count();

    [[nodiscard]] std::vector<Suit> get_remaining_bases() const;
    bool set_bases(const std::array<bool, static_cast<size_t>(Suit::kSuitCount) - 1> &bases);
    [[nodiscard]] bool is_base_remaining(Suit suit) const;
    bool set_base_remaining(Suit suit, bool isRemaining);
    [[nodiscard]] uint8_t get_remaining_sympathy_tokens() const;
    bool set_remaining_sympathy_tokens(uint8_t count);
    [[nodiscard]] std::vector<card_data::CardID> get_supporters() const;
    bool set_supporters(const std::vector<card_data::CardID> &supporters);
    [[nodiscard]] std::array<uint8_t, static_cast<size_t>(Suit::kSuitCount)> get_supporter_suits() const;
    [[nodiscard]] card_data::CardID get_card_from_supporters(uint8_t index) const;
    bool add_card_to_supporters(card_data::CardID card);
    bool remove_card_from_supporters(uint8_t index);
    bool draw_card_to_supporters();

    [[nodiscard]] uint8_t get_bag_count(FactionID whichVagabond) const;
    bool set_bag_count(FactionID whichVagabond, uint8_t count);
    [[nodiscard]] uint8_t get_boots_count(FactionID whichVagabond) const;
    bool set_boots_count(FactionID whichVagabond, uint8_t count);
    [[nodiscard]] uint8_t get_hammer_count(FactionID whichVagabond) const;
    bool set_hammer_count(FactionID whichVagabond, uint8_t count);
    [[nodiscard]] uint8_t get_sword_count(FactionID whichVagabond) const;
    bool set_sword_count(FactionID whichVagabond, uint8_t count);
    [[nodiscard]] uint8_t get_tea_count(FactionID whichVagabond) const;
    bool set_tea_count(FactionID whichVagabond, uint8_t count);
    [[nodiscard]] uint8_t get_coin_count(FactionID whichVagabond) const;
    bool set_coin_count(FactionID whichVagabond, uint8_t count);
    [[nodiscard]] uint8_t get_crossbow_count(FactionID whichVagabond) const;
    bool set_crossbow_count(FactionID whichVagabond, uint8_t count);
    [[nodiscard]] uint8_t has_torch(FactionID whichVagabond) const;
    bool set_has_torch(FactionID whichVagabond, bool hasTorch);
    [[nodiscard]] std::vector<vagabond::FactionRelationship> get_faction_relationships(FactionID whichVagabond);
    bool set_faction_relationships(FactionID whichVagabond, const std::vector<vagabond::FactionRelationship> &newFactionRelationships);
    [[nodiscard]] vagabond::FactionRelationship get_faction_relationship(FactionID whichVagabond, FactionID targetFactionID);
    bool set_faction_relationship(FactionID whichVagabond, vagabond::FactionRelationship newRelationship);
    [[nodiscard]] std::array<vagabond::QuestCardID, 15> get_quests() const;
    bool set_quests(const std::array<vagabond::QuestCardID, 15> &newQuests);
    void draw_quest();
    [[nodiscard]] std::array<vagabond::QuestCardID, 3> get_available_quests() const;
    bool complete_quest(FactionID whichVagabond, uint8_t index);

    [[nodiscard]] uint8_t get_acolyte_count() const;
    bool set_acolyte_count(uint8_t newCount);
    [[nodiscard]] std::array<uint8_t, 3> get_remaining_gardens() const;
    [[nodiscard]] uint8_t get_remaining_gardens(Suit suit) const;
    bool set_remaining_gardens(const std::array<uint8_t, 3> &newGardens);
    bool set_remaining_gardens(Suit suit, uint8_t newCount);
    [[nodiscard]] Suit get_outcast_suit() const;
    bool set_outcast_suit(Suit newSuit);
    [[nodiscard]] bool is_hated_outcast() const;
    bool set_is_hated_outcast(bool newIsHatedOutcast);
    [[nodiscard]] std::vector<card_data::CardID> get_lost_souls() const;
    bool set_lost_souls(const std::vector<card_data::CardID> &newLostSouls);

private:
    std::array<uint8_t, 112> data;
};

namespace marquise_de_cat
{


}

namespace eyrie
{
    enum class DecreeStack : uint8_t
    {
        kRecruit,
        kMove,
        kBattle,
        kBuild,
        DecreeStackCount
    };

    enum class Leader : uint8_t
    {
        kBuilder,
        kCharismatic,
        kCommander,
        kDespot
    };

    bool draw_1_if_hand_is_empty(FactionData &factionData);
    bool add_cards_to_decree(FactionData &factionData, bool isAI);
    bool craft(FactionData &factionData, bool isAI);
    bool resolve_the_decree(FactionData &factionData, bool isAI);
    bool recruit(FactionData &factionData, bool isAI, Suit suit);
    bool move(FactionData &factionData, bool isAI, Suit suit);
    bool battle(FactionData &factionData, bool isAI, Suit suit);
    bool build(FactionData &factionData, bool isAI, Suit suit);
    bool score(FactionData &factionData, bool isAI);
    bool draw_cards(FactionData &factionData, bool isAI);
    bool discard_cards(FactionData &factionData, bool isAI);
}

namespace woodland_alliance
{
    bool revolt_repeated(FactionData &factionData, bool isAI);
    bool spread_sympathy_repeated(FactionData &factionData, bool isAI);
    bool take_actions(FactionData &factionData, bool isAI);
    bool craft(FactionData &factionData, bool isAI);
    bool mobilize(FactionData &factionData, bool isAI);
    bool train(FactionData &factionData, bool isAI);
    bool conduct_military_operations(FactionData &factionData, bool isAI);
    bool move(FactionData &factionData, bool isAI);
    bool battle(FactionData &factionData, bool isAI);
    bool recruit(FactionData &factionData, bool isAI);
    bool organize(FactionData &factionData, bool isAI);
    bool draw_cards(FactionData &factionData, bool isAI);
    bool discard_cards(FactionData &factionData, bool isAI);

    bool on_outrage(FactionData &factionData, bool isAI, uint8_t triggerPlayerIndex);
    bool on_base_removed(FactionData &factionData, bool isAI, Suit removedBaseSuit);
}

namespace lizard_cult
{
    bool adjust_the_outcast(FactionData &factionData);
    bool discard_lost_souls(FactionData &factionData);
    bool perform_conspiracies(FactionData &factionData, bool isAI);
    bool convert(FactionData &factionData, bool isAI);
    bool crusade(FactionData &factionData, bool isAI);
    bool sanctify(FactionData &factionData, bool isAI);
    bool reveal_cards_for_rituals(FactionData &factionData, bool isAI);
    bool perform_rituals(FactionData &factionData);
    bool build(FactionData &factionData, bool isAI, Suit revealedSuit);
    bool recruit(FactionData &factionData, bool isAI, Suit revealedSuit);
    bool score(FactionData &factionData, bool isAI, Suit revealedSuit);
    bool sacrifice(FactionData &factionData, bool isAI);
    bool return_revealed_cards(FactionData &factionData, bool isAI);
    bool craft(FactionData &factionData, bool isAI);
    bool draw_cards(FactionData &factionData, bool isAI);
    bool discard_cards(FactionData &factionData, bool isAI);
}