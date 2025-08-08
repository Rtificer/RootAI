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
    kJewelryValue3
};

enum class RelicType : uint8_t
{
    kFigure,
    kTablet,
    kJewelry
};
} // token_data
} // game_data