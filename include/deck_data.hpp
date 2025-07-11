#pragma once
#include "game_types.hpp"
#include <array>

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
            using enum card_data::CardID;
            static constexpr std::array<card_data::CardID, kMaxDeckSize> kStartingCards;
            DeckData deckData{};
    };

    template<IsValidDeck DeckType>
    class ExilesAndPartisansDeck : public Deck<DeckType> {
        private:
            static constexpr std::array<card_data::CardID, kMaxDeckSize> kStartingCards;
            DeckData deckData;
    };
}
