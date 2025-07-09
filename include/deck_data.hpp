#pragma once
#include "game_types.hpp"
#include <array>

namespace deck_data
{
    static constexpr std::array<card_data::CardID, 54> kStandardDeck;
    static constexpr std::array<card_data::CardID, 54> kExilesAndPartisansDeck;
}
