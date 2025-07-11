#pragma once

#include "card_data.hpp"

#include <array>
#include <cstdint>
#include <vector>
#include <concepts>

namespace game_data
{
namespace deck_data
{
static constexpr uint8_t kMaxDeckSize = 54;
static constexpr uint8_t kDeckSizeBits = 6;

template <typename T>
concept IsValidDeck = requires {
    { T::kStartingCards } -> std::same_as<std::array<card_data::CardID, kMaxDeckSize>>;
};

template<IsValidDeck DeckType>
class Deck {
    public:
        [[nodiscard]] inline uint8_t get_deck_size() const;
        inline void set_deck_size(uint8_t newSize);

        [[nodiscard]] std::vector<card_data::CardID> get_deck_contents() const;
        void set_deck_contents(const std::vector<card_data::CardID> &newDeck);
        [[nodiscard]] std::vector<card_data::CardID> get_cards_in_deck(const std::vector<uint8_t> &desiredCardIndices) const;
        void set_cards_in_deck(const std::vector<std::pair<uint8_t, card_data::CardID>> &newIndexCardPairs);
        void add_cards_to_deck(const std::vector<card_data::CardID> &newCards);
        void remove_cards_from_deck(const std::vector<uint8_t> &indices);

    protected:
        using DeckData = std::array<uint8_t, (7 + kDeckSizeBits + card_data::kCardIDBits * kMaxDeckSize) / 8>;

        // Super unoptimized but its compile time and I don't care that much.
        static constexpr DeckData initialize_deck();
};

template<IsValidDeck DeckType>
class StandardDeck : public Deck<DeckType> {
    public:
        static constexpr std::array<card_data::CardID, kMaxDeckSize> kStartingCards = {
            // Dominance
            card_data::CardID::kMouseDominance,
            card_data::CardID::kFoxDominance,
            card_data::CardID::kRabbitDominance,
            card_data::CardID::kBirdDominance,

            // Ambush
            card_data::CardID::kMouseAmbush,
            card_data::CardID::kFoxAmbush,
            card_data::CardID::kRabbitAmbush,
            card_data::CardID::kBirdAmbush,
            card_data::CardID::kBirdAmbush,

            // Items & Gear
            card_data::CardID::kMouseInASack,
            card_data::CardID::kGentlyUsedKnapsack,
            card_data::CardID::kSmugglersTrail,
            card_data::CardID::kBirdyBindle,
            card_data::CardID::kMouseTravelGear,
            card_data::CardID::kFoxTravelGear,
            card_data::CardID::kAVisitToFriends,
            card_data::CardID::kWoodlandRunners,
            card_data::CardID::kAnvil,
            card_data::CardID::kSword,
            card_data::CardID::kFoxfolkSteel,
            card_data::CardID::kArmsTrader,
            card_data::CardID::kMouseRootTea,
            card_data::CardID::kFoxRootTea,
            card_data::CardID::kRabbitRootTea,
            card_data::CardID::kInvestments,
            card_data::CardID::kProtectionRacket,
            card_data::CardID::kBakeSale,
            card_data::CardID::kMouseCrossbow,
            card_data::CardID::kBirdCrossbow,

            // Standard deck unique cards
            card_data::CardID::kFavorOfTheMice,
            card_data::CardID::kFavorOfTheFoxes,
            card_data::CardID::kFavorOfTheRabbits,
            card_data::CardID::kCodebreakers,
            card_data::CardID::kCodebreakers,
            card_data::CardID::kScoutingParty,
            card_data::CardID::kScoutingParty,
            card_data::CardID::kStandAndDeliver,
            card_data::CardID::kStandAndDeliver,
            card_data::CardID::kTaxCollector,
            card_data::CardID::kTaxCollector,
            card_data::CardID::kTaxCollector,
            card_data::CardID::kBetterBurrowBank,
            card_data::CardID::kBetterBurrowBank,
            card_data::CardID::kCobbler,
            card_data::CardID::kCobbler,
            card_data::CardID::kCommandWarren,
            card_data::CardID::kCommandWarren,
            card_data::CardID::kArmorers,
            card_data::CardID::kArmorers,
            card_data::CardID::kBrutalTactics,
            card_data::CardID::kBrutalTactics,
            card_data::CardID::kRoyalClaim,
            card_data::CardID::kSappers,
            card_data::CardID::kSappers
        };
        typename Deck<DeckType>::DeckData deckData{};
};

template<IsValidDeck DeckType>
class ExilesAndPartisansDeck : public Deck<DeckType> {
    private:
        static constexpr std::array<card_data::CardID, kMaxDeckSize> kStartingCards = {
            // Dominance
            card_data::CardID::kMouseDominance,
            card_data::CardID::kFoxDominance,
            card_data::CardID::kRabbitDominance,
            card_data::CardID::kBirdDominance,

            // Ambush
            card_data::CardID::kMouseAmbush,
            card_data::CardID::kFoxAmbush,
            card_data::CardID::kRabbitAmbush,
            card_data::CardID::kBirdAmbush,
            card_data::CardID::kBirdAmbush,

            // Items & Gear
            card_data::CardID::kMouseInASack,
            card_data::CardID::kGentlyUsedKnapsack,
            card_data::CardID::kSmugglersTrail,
            card_data::CardID::kBirdyBindle,
            card_data::CardID::kMouseTravelGear,
            card_data::CardID::kFoxTravelGear,
            card_data::CardID::kAVisitToFriends,
            card_data::CardID::kWoodlandRunners,
            card_data::CardID::kAnvil,
            card_data::CardID::kSword,
            card_data::CardID::kFoxfolkSteel,
            card_data::CardID::kArmsTrader,
            card_data::CardID::kMouseRootTea,
            card_data::CardID::kFoxRootTea,
            card_data::CardID::kRabbitRootTea,
            card_data::CardID::kInvestments,
            card_data::CardID::kProtectionRacket,
            card_data::CardID::kBakeSale,
            card_data::CardID::kMouseCrossbow,
            card_data::CardID::kBirdCrossbow,

            // Exiles & Partisans unique cards
            card_data::CardID::kMousePartisans,
            card_data::CardID::kFoxPartisans,
            card_data::CardID::kRabbitPartisans,
            card_data::CardID::kLeagueOfAdventurousMice,
            card_data::CardID::kLeagueOfAdventurousMice,
            card_data::CardID::kMasterEngravers,
            card_data::CardID::kMurineBroker,
            card_data::CardID::kCharmOffensive,
            card_data::CardID::kCoffinMakers,
            card_data::CardID::kSwapMeet,
            card_data::CardID::kSwapMeet,
            card_data::CardID::kTunnels,
            card_data::CardID::kTunnels,
            card_data::CardID::kFalseOrders,
            card_data::CardID::kFalseOrders,
            card_data::CardID::kInformants,
            card_data::CardID::kInformants,
            card_data::CardID::kPropagandaBureau,
            card_data::CardID::kBoatBuilders,
            card_data::CardID::kCorvidPlanners,
            card_data::CardID::kEyrieÉmigré,
            card_data::CardID::kSaboteurs,
            card_data::CardID::kSaboteurs,
            card_data::CardID::kSaboteurs,
            card_data::CardID::kSoupKitchens
        };
        typename Deck<DeckType>::DeckData deckData;
};
}
}
