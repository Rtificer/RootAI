#pragma once

#include "card_data.hpp"
#include "deck_data.hpp"
#include "clearing_data.hpp"
#include "game_data.hpp"

#include <array>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <bit>
#include <stdexcept>
#include <vector>

namespace game_data
{
namespace faction_data 
{
enum class FactionID : uint8_t
{
    kMarquiseDeCat,     // A
    kEyrieDynasty,      // B
    kWoodlandAlliance,  // C
    kVagabond1,         // D
    kVagabond2,         // E
    kLizardCult,        // F
    kRiverfolkCompany,  // G
    kUndergroundDuchy,  // H
    kCorvidConspiracy,  // I
    kLordOfTheHundreds, // J
    kKeepersInIron,     // K
    kTotalFactions
};

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

    kMouseDominance = static_cast<uint8_t>(FactionID::kTotalFactions) + static_cast<uint8_t>(card_data::CardID::kMouseDominance),
    kFoxDominance = static_cast<uint8_t>(FactionID::kTotalFactions) + static_cast<uint8_t>(card_data::CardID::kFoxDominance),
    kRabbitDominance = static_cast<uint8_t>(FactionID::kTotalFactions) + static_cast<uint8_t>(card_data::CardID::kRabbitDominance),
    kBirdDominance = static_cast<uint8_t>(FactionID::kTotalFactions) + static_cast<uint8_t>(card_data::CardID::kBirdDominance),

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

template <typename FactionType> //CRTP (look up what it stands for its funny)
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

    template <::game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t shift, uint16_t width>
    [[nodiscard]] OutputType read_bits() const;
    template <::game_data::IsUnsignedIntegralOrEnum OutputType, uint16_t width>
    [[nodiscard]] OutputType read_bits(uint16_t shift) const;
    template <IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
    [[nodiscard]] OutputType read_bits() const;
    template <::game_data::IsValidByteArray OutputType, uint8_t elementWidth>
    [[nodiscard]] OutputType read_bits(uint16_t shift) const;

    template <::game_data::IsUnsignedIntegralOrEnum InputType, uint16_t shift, uint16_t width>
    void write_bits(const InputType &value);
    template <::game_data::IsUnsignedIntegralOrEnum InputType, uint16_t width>
    void write_bits(const InputType &value, uint16_t shift);
    template <::game_data::IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
    void write_bits(const InputType &value);
    template <::game_data::IsValidByteArray InputType, uint8_t elementWidth>
    void write_bits(const InputType, uint16_t shift);
    
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

    template <typename DeckType>
    inline void draw_cards(::game_data::deck_data::Deck<DeckType> &deck, uint8_t count);
    void discard_card_from_hand(uint8_t cardIndex);
    void build(::game_data::board_data::clearing_data::Clearing &desiredClearing, ::game_data::board_data::clearing_data::building_data::Building desiredBuilding);
    virtual void battle(uint8_t clearingIndex);
    virtual void move(uint8_t originClearingIndex, uint8_t destinationClearingIndex);
    virtual void recruit(uint8_t clearingIndex);
    virtual uint8_t calculate_extra_draws() = 0;

    std::array<uint8_t, 112> factionData;
};

class MarquiseDeCatFaction : public Faction<MarquiseDeCatFaction>
{
    public:
        static constexpr FactionID kFactionID = FactionID::kMarquiseDeCat;
        static constexpr uint8_t kPawnBits = 5;

        bool place_starting_wood(bool isAI);
        bool craft(bool isAI);
        bool take_actions(bool isAI);
        bool battle_action(bool isAI);
        bool march_action(bool isAI);
        bool recruit_action(bool isAI);
        bool build_action(bool isAI);
        bool overwork_action(bool isAI);
        bool draw_cards_end_of_turn(bool isAI);
        bool discard_cards(bool isAI);

    private:
        uint8_t calculate_extra_draws();
};
static_assert(IsValidFaction<MarquiseDeCatFaction>, "Marquise de cat faction is invalid");
} // faction_data
} // game_data