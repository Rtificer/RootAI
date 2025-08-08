#pragma once

#include "card_data.hpp"
#include "deck_data.hpp"
#include "discard_pile_data.hpp"
#include "clearing_data.hpp"
#include "game_data.hpp"

#include <array>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <bit>
#include <stdexcept>
#include <vector>
#include <random>
#include <variant>

namespace game_data
{
namespace faction_data 
{
// FactionID moved to game_data.hpp

enum class ExpandedScore : uint8_t
{
    // Maintaining Continuity with factionID is more important than maintaining continuity with CardID, since the position of each could be moved.
    kAlliedWithMarquiseDeCat = static_cast<uint8_t>(FactionID::kMarquiseDeCat),
    kAlliedWithEyrieDynasty = static_cast<uint8_t>(FactionID::kEyrieDynasty),
    kAlliedWithWoodlandAlliance = static_cast<uint8_t>(FactionID::kWoodlandAlliance),
    kAlliedWithLizardCult = static_cast<uint8_t>(FactionID::kLizardCult),
    kAlliedWithRiverfolkCompany = static_cast<uint8_t>(FactionID::kRiverfolkCompany),
    kAlliedWithUndergroundDuchy = static_cast<uint8_t>(FactionID::kUndergroundDuchy),
    kAlliedWithCorvidConspiracy = static_cast<uint8_t>(FactionID::kCorvidConspiracy),
    kAlliedWithLordOfTheHundreds = static_cast<uint8_t>(FactionID::kLordOfTheHundreds),
    kAlliedWithKeepersInIron = static_cast<uint8_t>(FactionID::kKeepersInIron),

    kMouseDominance = static_cast<uint8_t>(FactionID::kKeepersInIron) + static_cast<uint8_t>(card_data::CardID::kMouseDominance) + 1,
    kFoxDominance = static_cast<uint8_t>(FactionID::kKeepersInIron) + static_cast<uint8_t>(card_data::CardID::kFoxDominance) + 1,
    kRabbitDominance = static_cast<uint8_t>(FactionID::kKeepersInIron) + static_cast<uint8_t>(card_data::CardID::kRabbitDominance) + 1,
    kBirdDominance = static_cast<uint8_t>(FactionID::kKeepersInIron) + static_cast<uint8_t>(card_data::CardID::kBirdDominance) + 1,

    k0 = std::max({kMouseDominance, kFoxDominance, kRabbitDominance, kBirdDominance}) + 1,
    k1,
    k2,
    k3,
    k4,
    k5,
    k6,
    k7,
    k8,
    k9,
    k10,
    k11,
    k12,
    k13,
    k14,
    k15,
    k16,
    k17,
    k18,
    k19,
    k20,
    k21,
    k22,
    k23,
    k24,
    k25,
    k26,
    k27,
    k28,
    k29,
    k30,
};

template <typename T>
concept IsValidFaction = requires {
    { T::kFactionID } -> std::convertible_to<FactionID>;
};

template <typename T>
concept HasPawns = requires {
    { T::kPawnBits } -> std::same_as<uint8_t>;
} && (T::kPawnBits > 0);

template <typename T>
concept IsVagabond = requires {
    { T::kFactionID } -> std::convertible_to<FactionID>;
} && (T::kFactionID == FactionID::kVagabond1 || T::kFactionID == FactionID::kVagabond2);

template <typename FactionType, bool isAI> //CRTP (look up what it stands for its funny)
class Faction
{
public:
    virtual bool draw_cards_turn_end();
    virtual bool discard_cards_turn_end();

    virtual ~Faction() = default;

protected:
    static constexpr uint8_t kScoreBits = 5;
    static constexpr uint8_t kMaxHandSize = 18;
    static constexpr uint8_t kHandWriteIndexBits = 5;
    static constexpr uint8_t kHandContentBits = card_data::kCardIDBits * kMaxHandSize + kHandWriteIndexBits;
    static constexpr uint8_t kHandSizeBits = 5;

    static constexpr uint16_t kScoreOffset = 0;
    static constexpr uint16_t kHandContentOffset = kScoreOffset + kScoreBits;
    static constexpr uint16_t kHandSizeOffset = kHandContentOffset + kHandContentBits;
    static constexpr uint16_t kPawnOffset = kHandSizeOffset + kHandSizeBits;
    
    [[nodiscard]] inline ExpandedScore get_score() const;
    inline void set_score(ExpandedScore newScore);

    [[nodiscard]] inline uint8_t get_hand_size() const;
    inline void set_hand_size(uint8_t newSize);
    template <uint8_t newSize>
    inline void set_hand_size();
    [[nodiscard]] std::vector<card_data::CardID> get_hand_contents() const;
    void set_hand_contents(const std::vector<card_data::CardID> &newHand);
    [[nodiscard]] std::vector<card_data::CardID> get_cards_in_hand(const std::vector<uint8_t> &desiredCardIndices) const;
    void set_cards_in_hand(const std::vector<std::pair<uint8_t, card_data::CardID>> &newIndexCardPairs);
    void add_cards_to_hand(const std::vector<card_data::CardID> &newCards);
    void remove_cards_from_hand(const std::vector<uint8_t> &indices);

    [[nodiscard]] inline uint8_t get_remaining_pawn_count() const requires HasPawns<FactionType>;
    inline void set_remaining_pawn_count(uint8_t newCount) requires HasPawns<FactionType>;

    template <::game_data::deck_data::DeckType deckType>
    inline void draw_cards(::game_data::deck_data::Deck<deckType> &deck, /*::game_data::discard_pile_data::DiscardPile &discard_pile, std::mt19937& engine,*/ uint8_t count);
    inline void discard_card_from_hand(::game_data::discard_pile_data::DiscardPile &discard_pile, uint8_t cardIndex);
    virtual void battle(uint8_t clearingIndex);
    virtual void move(uint8_t originClearingIndex, uint8_t destinationClearingIndex);
    virtual void recruit(uint8_t clearingIndex);
    virtual uint8_t calculate_extra_draws() = 0;

    std::array<uint8_t, 112> factionData;

        // Wrappers for read and write bits functions to allow for ease of use
        template <::game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t shift, uint16_t width>
        [[nodiscard]] inline OutputType read_bits() const {
            return ::game_data::read_bits<OutputType, sizeof(factionData), shift, width>(factionData);
        }

        template <::game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t width>
        [[nodiscard]] inline OutputType read_bits(uint16_t shift) const {
            return ::game_data::read_bits<OutputType, sizeof(factionData), width>(factionData, shift);
        }

        template <::game_data::IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
        [[nodiscard]] inline OutputType read_bits() const {
            return ::game_data::read_bits<OutputType, sizeof(factionData), shift, elementWidth>(factionData);
        }

        template <::game_data::IsValidByteArray OutputType, uint8_t elementWidth>
        [[nodiscard]] inline OutputType read_bits(uint16_t shift) const {
            return ::game_data::read_bits<OutputType, sizeof(factionData), elementWidth>(factionData, shift);
        }

        template <::game_data::IsUnsignedIntegralOrEnum InputType, uint16_t shift, uint16_t width>
        inline void write_bits(const InputType &value) {
            ::game_data::write_bits<InputType, sizeof(factionData), shift, width>(factionData, value);
        }

        template <::game_data::IsUnsignedIntegralOrEnum InputType, uint16_t width>
        inline void write_bits(const InputType &value, uint16_t shift) {
            ::game_data::write_bits<InputType, sizeof(factionData), width>(factionData, value, shift);
        }

        template <::game_data::IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
        inline void write_bits(const InputType &value) {
            ::game_data::write_bits<InputType, sizeof(factionData), shift, elementWidth>(factionData, value);
        }

        template <::game_data::IsValidByteArray InputType, uint8_t elementWidth>
        inline void write_bits(const InputType &value, uint16_t shift) {
            ::game_data::write_bits<InputType, sizeof(factionData), elementWidth>(factionData, value, shift);
        }
};

template <bool isAI>
class MarquiseDeCatFaction : public Faction<MarquiseDeCatFaction<isAI>, isAI>
{
    public:
        static constexpr FactionID kFactionID = FactionID::kMarquiseDeCat;
        static constexpr uint8_t kPawnBits = 5;

        bool place_starting_wood();
        bool craft();
        bool take_actions();
        bool battle_action();
        bool march_action();
        bool recruit_action();
        bool build_action();
        bool overwork_action();
        bool draw_cards_end_of_turn();
        bool discard_cards();

    private:
        uint8_t calculate_extra_draws();
};

// stupid hack
static_assert(IsValidFaction<MarquiseDeCatFaction<true>>, "Marquise de cat faction is invalid");
static_assert(IsValidFaction<MarquiseDeCatFaction<false>>, "Marquise de cat faction is invalid");

using FactionVariant = std::variant<
    MarquiseDeCatFaction<true>,
    MarquiseDeCatFaction<false>
>;
} // faction_data
} // game_data