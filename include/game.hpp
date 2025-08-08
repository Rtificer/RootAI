#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <exception>

#include "game_data.hpp"
#include "board_data.hpp"
#include "factions_data.hpp"

namespace game_data
{
template <uint8_t playerCount, std::array<bool, playerCount> isAIArray>
class Game
{
    Game(
        board_data::BoardType boardType,
        deck_data::DeckType deckType,
        SetupType setupType,
        const std::optional<std::array<faction_data::FactionID, playerCount>> &factions
    )
    {

        std::variant<
            deck_data::Deck<deck_data::DeckType::kStandard>,
            deck_data::Deck<deck_data::DeckType::kExilesAndPartisans>
        > deck;

        if (deckType == deck_data::DeckType::kStandard) {
            deck = deck_data::Deck<deck_data::DeckType::kStandard>();
        } else {
            deck = deck_data::Deck<deck_data::DeckType::kExilesAndPartisans>();
        }

        if (setupType == SetupType::Normal) {
            [[unlikely]] if (!factions) {
                throw std::invalid_argument("Faction array cannot be empty when using standard setup.")
            }
        } else { // Advanced Setup
            
        }
    }
};
}
