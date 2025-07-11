#include "../include/deck_data.hpp"

namespace deck_data
{
    template <IsValidDeck DeckType>
    constexpr Deck<DeckType>::DeckData Deck<DeckType>::initialize_deck()
    {
        DeckData data{};
        // Set deck size bits
        uint16_t bitPos = 0;
        uint8_t deckSize = kMaxDeckSize;
        for (uint8_t bit = 0; bit < kDeckSizeBits; ++i, ++bitPos)
        {
            if (deckSize & (1 << i))
                data[bitPos / 8] |= (1 << (bitPos % 8));
        }
        // Set card IDs
        for (uint8_t cardIdx = 0; cardIdx < kMaxDeckSize; ++cardIdx)
        {
            uint8_t cardID = static_cast<uint8_t>(kStartingCards[cardIdx]);
            for (uint8_t bit = 0; bit < card_data::kCardIDBits; ++bit, ++bitPos)
            {
                if (cardID & (1 << bit))
                    data[bitPos / 8] |= (1 << (bitPos % 8));
            }
        }
        return data;
    }

    using enum card_data::CardID;
    template<IsValidDeck DeckType>
    constexpr std::array<card_data::CardID, kMaxDeckSize> StandardDeck<DeckType>::kStartingCards = {
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
        kBirdAmbush,

        // Items & Gear
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

        // Standard deck unique cards
        kFavorOfTheMice,
        kFavorOfTheFoxes,
        kFavorOfTheRabbits,
        kCodebreakers,
        kCodebreakers,
        kScoutingParty,
        kScoutingParty,
        kStandAndDeliver,
        kStandAndDeliver,
        kTaxCollector,
        kTaxCollector,
        kTaxCollector,
        kBetterBurrowBank,
        kBetterBurrowBank,
        kCobbler,
        kCobbler,
        kCommandWarren,
        kCommandWarren,
        kArmorers,
        kArmorers,
        kBrutalTactics,
        kBrutalTactics,
        kRoyalClaim,
        kSappers,
        kSappers
    };
    
    template<IsValidDeck DeckType>
    static constexpr std::array<card_data::CardID, kMaxDeckSize> ExilesAndPartisansDeck<DeckType>::kStartingCards = {
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
        kBirdAmbush,

        // Items & Gear
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

        // Exiles & Partisans unique cards
        kMousePartisans,
        kFoxPartisans,
        kRabbitPartisans,
        kLeagueOfAdventurousMice,
        kLeagueOfAdventurousMice,
        kMasterEngravers,
        kMurineBroker,
        kCharmOffensive,
        kCoffinMakers,
        kSwapMeet,
        kSwapMeet,
        kTunnels,
        kTunnels,
        kFalseOrders,
        kFalseOrders,
        kInformants,
        kInformants,
        kPropagandaBureau,
        kBoatBuilders,
        kCorvidPlanners,
        kEyrieÉmigré,
        kSaboteurs,
        kSaboteurs,
        kSaboteurs,
        kSoupKitchens
    };

}