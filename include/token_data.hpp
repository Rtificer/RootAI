#pragma once

#include <cstdint>

namespace game_data
{
namespace token_data
{
enum class Token : uint8_t
{
    kWood,
    kKeep,
    kSympathy,
    kMouseTradePost,
    kFoxTradePost,
    kRabbitTradePost,
    kTunnel,
    kBombPlot,
    kSnarePlot,
    kExtortionPlot,
    kRaidPlot,
    kMob,
    kFigureValue1,
    kFigureValue2,
    kFigureValue3,
    kTabletValue1,
    kTabletValue2,
    kTabletValue3,
    kJewelryValue1,
    kJewelryValue2,
    kJewelryValue3,
    kMaxTokenIndex // for bounds checking
};

struct TokenInfo
{
    uint8_t offset;
    uint8_t width;
};

static constexpr TokenInfo kTokenFieldInfo[] = {
    {0, 3},  // Wood
    {3, 1},  // Keep
    {4, 1},  // Sympathy
    {5, 1},  // Mouse Trade Post
    {6, 1},  // Fox Trade Post
    {7, 1},  // Rabbit Trade Post
    {8, 1},  // Tunnel
    {9, 1},  // Bomb Plot
    {10, 2}, // Snare Plot
    {12, 2}, // Extortion Plot
    {15, 2}, // Raid Plot
    {17, 1}, // Mob
    {18, 1}, // Figure Value 1
    {19, 1}, // Figure Value 2
    {20, 2}, // Figure Value 3
    {22, 1}, // Tablet Value 1
    {23, 1}, // Tablet Value 2
    {24, 2}, // Tablet Value 3
    {26, 1}, // Jewelry Value 1
    {27, 1}, // Jewelry Value 2
    {28, 2}, // Jewelry Value 3
};
} // token_data
} // game_data