#pragma once

#include "game_types.hpp"

#include <array>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <bit>
#include <stdexcept>

namespace faction_data
{
    enum class FactionID : uint8_t;
    enum class ExpandedScore : uint8_t;

    template <typename T>
    concept IsValidFaction = requires {
        { T::kFactionID } -> std::same_as<FactionID>;
    };
    
    template<typename T>
    using Clean_t = std::remove_cv_t<std::remove_reference_t<T>>;

    template<typename T>
    concept IsUnsignedIntegralOrEnum = std::unsigned_integral<T> || 
        (std::is_enum_v<T> && std::unsigned_integral<std::underlying_type_t<T>>);


    // Note: I had problems with template metaprograming when dealing with const so I spammed Clean_t everywhere. Some uses might be unnecessary but idc tbh its good enough
    template<typename T>
    concept IsValidByteArray =
        // Check that T is an std::array or std::vector of some type and size
        (std::is_same_v<T, std::array<Clean_t<typename T::value_type>, std::tuple_size_v<T>>> && std::tuple_size_v<T> > 0 ) &&
        (
            // The element type is uint8_t
            std::is_same_v<Clean_t<typename T::value_type>, uint8_t> ||
            // Or, the element type is an enum whose underlying type is uint8_t
            (std::is_enum_v<Clean_t<typename T::value_type>> && std::is_same_v<std::underlying_type_t<Clean_t<typename T::value_type>>, uint8_t>)
        );
    
    template <typename T>
    concept HasPawns = requires {
        { T::kPawnBits } -> std::same_as<uint8_t>;
    } && (T::kPawnBits > 0);

    template <typename T>
    concept IsVagabond = requires {
        { T::kFactionID } -> std::same_as<FactionID>;
    } && (T::kFactionID == FactionID::kVagabond1 || T::kFactionID == FactionID::kVagabond2);

    template <IsValidFaction FactionType>
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
        static constexpr uint16_t kHandSizeOffset = kHandOffset + kHandContentBits;
        static constexpr uint16_t kPawnOffset = kHandSizeOffset + kHandSizeBits;

        template <IsUnsignedIntegralOrEnum OutputType, uint16_t shift, uint16_t width>
        [[nodiscard]] OutputType read_bits() const;
        template <IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
        [[nodiscard]] OutputType read_bits() const;

        template<IsUnsignedIntegralOrEnum InputType, uint16_t shift, uint16_t width>
        void write_bits(const InputType &value);
        template <IsValidByteArray InputType, uint16_t shift, uint8_t elementWidth>
        void write_bits(const InputType &value);
        
        [[nodiscard]] inline ExpandedScore get_score() const;
        inline void set_score(ExpandedScore newScore);

        [[nodiscard]] inline uint8_t get_hand_size() const;
        inline void set_hand_size(uint8_t newSize);
        template<uint8_t newSize>
        inline void set_hand_size();
        [[nodiscard]] std::vector<card_data::CardID> get_hand_contents() const;
        void set_hand_contents(const std::vector<card_data::CardID> &newHand);
        [[nodiscard]] std::vector<card_data::CardID> get_cards_in_hand(const std::vector<uint8_t> &desiredCardIndices) const;
        void set_cards_in_hand(const std::vector<std::pair<uint8_t, card_data::CardID>> &newIndexCardPairs);
        void add_cards_to_hand(const std::vector<card_data::CardID> &newCards);
        void remove_cards_from_hand(const std::vector<uint8_t> &indices);

        [[nodiscard]] inline uint8_t get_remaining_pawn_count() const requires HasPawns<FactionType>;
        inline void set_remaining_pawn_count(uint8_t newCount) requires HasPawns<FactionType>;

        void draw_cards(std::array<card_data::CardID, 54> &deck, uint8_t &deckSize, uint8_t count);
        void discard_card_from_hand(uint8_t cardIndex);
        void build(Clearing &desiredClearing, Building desiredBuilding);
        virtual void battle(uint8_t clearingIndex);
        virtual void move(uint8_t originClearingIndex, uint8_t destinationClearingIndex);
        virtual void recruit(uint8_t clearingIndex);
        virtual uint8_t calculate_extra_draws() = 0;

        std::array<uint8_t, 112> factionData;
    };

    template<IsValidFaction FactionType>
    class MarquiseDeCatFaction : public Faction<MarquiseDeCatFaction>
    {
        public:
            static constexpr FactionID kFactionID = FactionID::kMarquiseDeCat;
            static constexpr uint8_t kPawnBits = 5;

            bool place_starting_wood(FactionData &factionData, bool isAI);
            bool craft(bool isAI);
            bool take_actions(bool isAI);
            bool battle(bool isAI);
            bool march(bool isAI);
            bool recruit(bool isAI);
            bool build(bool isAI);
            bool overwork(bool isAI);
            bool draw_cards_end_of_turn(bool isAI);
            bool discard_cards(FactionData &factionData, bool isAI);

        private:
            uint8_t calculate_extra_draws() override;


    };
}