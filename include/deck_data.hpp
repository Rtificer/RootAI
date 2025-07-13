#pragma once

#include "card_data.hpp"
#include "game_data.hpp"

#include <array>
#include <cstdint>
#include <vector>
#include <concepts>
#include <stdexcept>
#include <algorithm>
#include <cstring>

namespace game_data
{
namespace deck_data
{

template <typename T>
concept IsValidDeck = requires {
    { T::kStartingCards } -> std::same_as<std::array<::game_data::card_data::CardID, ::game_data::card_data::kTotalCards>>;
};

template<IsValidDeck DeckType>
class Deck {
    public:
        template<::game_data::IsUint8OrEnumUint8 OutputType, uint16_t shift, uint8_t width>
        [[nodiscard]] OutputType read_bits() const;
        template<::game_data::IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
        [[nodiscard]] OutputType read_bits() const;

        template<::game_data::IsUint8OrEnumUint8 InputType, uint16_t shift, uint16_t width>
        void write_bits(const InputType &value);
        template<::game_data::IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
        void write_bits(const InputType &value);


    
        [[nodiscard]] inline uint8_t get_deck_size() const;
        template <uint8_t newSize>
        inline void set_deck_size();
        inline void set_deck_size(uint8_t newSize);

        [[nodiscard]] std::vector<::game_data::card_data::CardID> get_deck_contents() const;
        void set_deck_contents(const std::vector<::game_data::card_data::CardID> &newDeck);
        [[nodiscard]] std::vector<::game_data::card_data::CardID> get_cards_in_deck(const std::vector<uint8_t> &desiredCardIndices) const;
        [[nodiscard]] std::vector<::game_data::card_data::CardID> get_cards_in_deck(uint8_t startIndex, uint8_t endIndex) const;
        void set_cards_in_deck(const std::vector<std::pair<uint8_t, ::game_data::card_data::CardID>> &newIndexCardPairs);
        void add_cards_to_deck(const std::vector<::game_data::card_data::CardID> &newCards);
        void remove_cards_from_deck(const std::vector<uint8_t> &indices);
        inline void pop_cards_from_deck(uint8_t popCardCount);

    protected:
        static constexpr uint8_t kDeckSizeBits = 6;
        static constexpr uint16_t kDeckContentBits = ::game_data::card_data::kTotalCards * ::game_data::card_data::kCardIDBits;

        static constexpr uint16_t kDeckSizeOffset = 0;
        static constexpr uint16_t kDeckContentOffset = kDeckSizeOffset + kDeckSizeBits;

        using DeckData = std::array<uint8_t, (7 + kDeckSizeBits + card_data::kCardIDBits * ::game_data::card_data::kTotalCards) / 8>;

        // Super unoptimized but its compile time and I don't care that much.
        static constexpr DeckData initialize_deck();
        typename Deck<DeckType>::DeckData deckData = initialize_deck();

        
};

template<IsValidDeck DeckType>
class StandardDeck : public Deck<DeckType> {
    public:
        static constexpr std::array<::game_data::card_data::CardID, ::game_data::card_data::kTotalCards> kStartingCards = {
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
};

template<IsValidDeck DeckType>
class ExilesAndPartisansDeck : public Deck<DeckType> {
    private:
        static constexpr std::array<::game_data::card_data::CardID, ::game_data::card_data::kTotalCards> kStartingCards = {
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
