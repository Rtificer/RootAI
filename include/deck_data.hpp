#pragma once

#include "card_pile.hpp"
#include "card_data.hpp"
#include "discard_pile_data.hpp"

#include <array>
#include <cstdint>
#include "Random123/threefry.h"



namespace game_data
{
namespace deck_data
{

using namespace ::game_data::discard_pile_data;

enum class DeckType
{
    kStandard,
    kExilesAndPartisans
};

template <DeckType deckType>
class Deck : public ::game_data::pile_data::CardPile
{
public:
    Deck(
        r123::Threefry2x32_R<12>::ctr_type &ctr,
        const r123::Threefry2x32_R<12>::key_type &key
    ) : CardPile(), ctr(ctr), key(key) {
        // Apparently using this-> is good practice here or something
        this->pileData = this->initialize_pile();
    }

    [[nodiscard]] consteval std::expected<void, pile_data::PileError> shuffle();

protected:
    r123::Threefry2x32_R<12>::ctr_type &ctr;
    const r123::Threefry2x32_R<12>::key_type &key;

    template <size_t Bit, size_t Max>
    consteval void set_deck_size_bits(CardPileData& data, uint16_t& bitPos) const;
    
    template <size_t CardIdx, size_t Max>
    consteval void set_card_ids(CardPileData& data, uint16_t& bitPos) const;

    void on_pile_empty() override
    {
        
    }

    [[nodiscard]] inline consteval std::expected<void, pile_data::PileError> turnover_discard(discard_pile_data::DiscardPile &discardPile);

    [[nodiscard]] consteval CardPileData initialize_pile() const override; 

    static constexpr std::array<::game_data::card_data::CardID, ::game_data::card_data::kTotalCards> kStandardStartingCards = {
        // Dominance
        ::game_data::card_data::CardID::kMouseDominance,
        ::game_data::card_data::CardID::kFoxDominance,
        ::game_data::card_data::CardID::kRabbitDominance,
        ::game_data::card_data::CardID::kBirdDominance,

        // Ambush
        ::game_data::card_data::CardID::kMouseAmbush,
        ::game_data::card_data::CardID::kFoxAmbush,
        ::game_data::card_data::CardID::kRabbitAmbush,
        ::game_data::card_data::CardID::kBirdAmbush,
        ::game_data::card_data::CardID::kBirdAmbush,

        // Items & Gear
        ::game_data::card_data::CardID::kMouseInASack,
        ::game_data::card_data::CardID::kGentlyUsedKnapsack,
        ::game_data::card_data::CardID::kSmugglersTrail,
        ::game_data::card_data::CardID::kBirdyBindle,
        ::game_data::card_data::CardID::kMouseTravelGear,
        ::game_data::card_data::CardID::kFoxTravelGear,
        ::game_data::card_data::CardID::kAVisitToFriends,
        ::game_data::card_data::CardID::kWoodlandRunners,
        ::game_data::card_data::CardID::kAnvil,
        ::game_data::card_data::CardID::kSword,
        ::game_data::card_data::CardID::kFoxfolkSteel,
        ::game_data::card_data::CardID::kArmsTrader,
        ::game_data::card_data::CardID::kMouseRootTea,
        ::game_data::card_data::CardID::kFoxRootTea,
        ::game_data::card_data::CardID::kRabbitRootTea,
        ::game_data::card_data::CardID::kInvestments,
        ::game_data::card_data::CardID::kProtectionRacket,
        ::game_data::card_data::CardID::kBakeSale,
        ::game_data::card_data::CardID::kMouseCrossbow,
        ::game_data::card_data::CardID::kBirdCrossbow,

        // Standard deck unique cards
        ::game_data::card_data::CardID::kFavorOfTheMice,
        ::game_data::card_data::CardID::kFavorOfTheFoxes,
        ::game_data::card_data::CardID::kFavorOfTheRabbits,
        ::game_data::card_data::CardID::kCodebreakers,
        ::game_data::card_data::CardID::kCodebreakers,
        ::game_data::card_data::CardID::kScoutingParty,
        ::game_data::card_data::CardID::kScoutingParty,
        ::game_data::card_data::CardID::kStandAndDeliver,
        ::game_data::card_data::CardID::kStandAndDeliver,
        ::game_data::card_data::CardID::kTaxCollector,
        ::game_data::card_data::CardID::kTaxCollector,
        ::game_data::card_data::CardID::kTaxCollector,
        ::game_data::card_data::CardID::kBetterBurrowBank,
        ::game_data::card_data::CardID::kBetterBurrowBank,
        ::game_data::card_data::CardID::kCobbler,
        ::game_data::card_data::CardID::kCobbler,
        ::game_data::card_data::CardID::kCommandWarren,
        ::game_data::card_data::CardID::kCommandWarren,
        ::game_data::card_data::CardID::kArmorers,
        ::game_data::card_data::CardID::kArmorers,
        ::game_data::card_data::CardID::kBrutalTactics,
        ::game_data::card_data::CardID::kBrutalTactics,
        ::game_data::card_data::CardID::kRoyalClaim,
        ::game_data::card_data::CardID::kSappers,
        ::game_data::card_data::CardID::kSappers
    };

    static constexpr std::array<::game_data::card_data::CardID, ::game_data::card_data::kTotalCards> kExilesAndPartisansStartingCards = {
        // Dominance
        ::game_data::card_data::CardID::kMouseDominance,
        ::game_data::card_data::CardID::kFoxDominance,
        ::game_data::card_data::CardID::kRabbitDominance,
        ::game_data::card_data::CardID::kBirdDominance,

        // Ambush
        ::game_data::card_data::CardID::kMouseAmbush,
        ::game_data::card_data::CardID::kFoxAmbush,
        ::game_data::card_data::CardID::kRabbitAmbush,
        ::game_data::card_data::CardID::kBirdAmbush,
        ::game_data::card_data::CardID::kBirdAmbush,

        // Items & Gear
        ::game_data::card_data::CardID::kMouseInASack,
        ::game_data::card_data::CardID::kGentlyUsedKnapsack,
        ::game_data::card_data::CardID::kSmugglersTrail,
        ::game_data::card_data::CardID::kBirdyBindle,
        ::game_data::card_data::CardID::kMouseTravelGear,
        ::game_data::card_data::CardID::kFoxTravelGear,
        ::game_data::card_data::CardID::kAVisitToFriends,
        ::game_data::card_data::CardID::kWoodlandRunners,
        ::game_data::card_data::CardID::kAnvil,
        ::game_data::card_data::CardID::kSword,
        ::game_data::card_data::CardID::kFoxfolkSteel,
        ::game_data::card_data::CardID::kArmsTrader,
        ::game_data::card_data::CardID::kMouseRootTea,
        ::game_data::card_data::CardID::kFoxRootTea,
        ::game_data::card_data::CardID::kRabbitRootTea,
        ::game_data::card_data::CardID::kInvestments,
        ::game_data::card_data::CardID::kProtectionRacket,
        ::game_data::card_data::CardID::kBakeSale,
        ::game_data::card_data::CardID::kMouseCrossbow,
        ::game_data::card_data::CardID::kBirdCrossbow,

        // Exiles & Partisans unique cards
        ::game_data::card_data::CardID::kMousePartisans,
        ::game_data::card_data::CardID::kFoxPartisans,
        ::game_data::card_data::CardID::kRabbitPartisans,
        ::game_data::card_data::CardID::kLeagueOfAdventurousMice,
        ::game_data::card_data::CardID::kLeagueOfAdventurousMice,
        ::game_data::card_data::CardID::kMasterEngravers,
        ::game_data::card_data::CardID::kMurineBroker,
        ::game_data::card_data::CardID::kCharmOffensive,
        ::game_data::card_data::CardID::kCoffinMakers,
        ::game_data::card_data::CardID::kSwapMeet,
        ::game_data::card_data::CardID::kSwapMeet,
        ::game_data::card_data::CardID::kTunnels,
        ::game_data::card_data::CardID::kTunnels,
        ::game_data::card_data::CardID::kFalseOrders,
        ::game_data::card_data::CardID::kFalseOrders,
        ::game_data::card_data::CardID::kInformants,
        ::game_data::card_data::CardID::kInformants,
        ::game_data::card_data::CardID::kPropagandaBureau,
        ::game_data::card_data::CardID::kBoatBuilders,
        ::game_data::card_data::CardID::kCorvidPlanners,
        ::game_data::card_data::CardID::kEyrieÉmigré,
        ::game_data::card_data::CardID::kSaboteurs,
        ::game_data::card_data::CardID::kSaboteurs,
        ::game_data::card_data::CardID::kSaboteurs,
        ::game_data::card_data::CardID::kSoupKitchens
    };
};
} // deck_data
} // game_data