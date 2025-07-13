#pragma once

#include <cstdint>

namespace game_data
{
namespace card_data
{
static constexpr uint8_t kCardIDBits = 6;
static constexpr uint8_t kTotalCards = 54;

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
} // card_data
} // game_data