#pragma once

#include "game_types.hpp"

#include <array>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <bit>

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

    template<typename T>
    concept IsValidByteArrayOrVector =
        // Check that T is an std::array or std::vector of some type and size
        (std::is_same_v<T, std::array<Clean_t<typename T::value_type>, std::tuple_size_v<T>>> && std::tuple_size_v<T> > 0 )|| 
        (std::is_same_v<T, std::vector<Clean_t<typename T::value_type>>>) &&
        (
            // The element type is uint8_t
            std::is_same_v<Clean_t<typename T::value_type>, uint8_t> ||
            // Or, the element type is an enum whose underlying type is uint8_t
            (std::is_enum_v<Clean_t<typename T::value_type>> && std::is_same_v<std::underlying_type_t<Clean_t<typename T::value_type>>, uint8_t>)
        );

    
    template <typename T>
    concept HasPawns = requires {
        { T::kPawnBits } -> std::same_as<uint8_t>;
        T::kPawnBits > 0;
    };

    template <typename T>
    concept IsVagabond = requires {
        T::kFactionID == FactionID::kVagabond1 || T::kFactionID == FactionID::kVagabond2;
    };

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
        static constexpr uint8_t kHandBits = card_data::kCardIDBits * kMaxHandSize + kHandWriteIndexBits;

        static constexpr uint16_t kScoreOffset = 0;
        static constexpr uint16_t kHandOffset = kScoreOffset + kHandBits;

        template<IsUnsignedIntegralOrEnum OutputType, uint16_t... Indices>
        OutputType unroll_bytes(uint8_t byteIndex, std::index_sequence<Indices...>) const;

        template <IsUnsignedIntegralOrEnum OutputType, uint16_t shift, uint16_t width>
        [[nodiscard]] OutputType read_bits() const;
        template <IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
        [[nodiscard]] OutputType read_bits() const;

        template <IsUnsignedIntegralOrEnum InputType>
        void write_bits(const InputType &value, uint16_t shift, uint16_t width);
        template <IsValidByteArray OutputType, uint16_t shift, uint8_t elementWidth>
        void write_bits(const InputType &value, uint16_t shift, uint8_t element_width);
        

        [[nodiscard]] std::vector<card_data::CardID> get_cards_in_hand(std::vector<uint8_t> desiredCardIndices);
        bool set_cards_in_hand(std::vector<std::pair<uint8_t, card_data::CardID>> newIndexCardPairs);
        bool add_cards_to_hand(std::vector<card_data::CardID> newCards);
        bool remove_cards_from_hand(std::vector<uint8_t> indices);

        [[nodiscard]] uint8_t get_remaining_pawn_count() const requires HasPawns<FactionType>;
        bool set_remaining_pawn_count(uint8_t newCount) requires HasPawns<FactionType>;
        
        [[nodiscard]] ExpandedScore get_score() const;
        bool set_score(ExpandedScore newScore);

        bool draw_cards(std::array<card_data::CardID, 54> &deck, uint8_t count);
        bool discard_card_from_hand(uint8_t cardIndex);
        bool build(Clearing &desiredClearing. Building desiredBuilding);
        virtual bool battle(uint8_t clearingIndex);
        virtual bool move(uint8_t originClearingIndex, uint8_t destinationClearingIndex);
        virtual bool recruit(uint8_t clearingIndex);
        virtual uint8_t calculate_extra_draws() = 0;

        std::array<uint8_t, 112> factionData;
    };

    class MarquiseDeCatFaction : public Faction<MarquiseDeCatFaction>
    {
        public:
            static constexpr FactionID kFactionID = FactionID::kMarquiseDeCat;

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
            static constexpr uint8_t kPawnBits = 5;
            uint8_t calculate_extra_draws() override;
    };
}