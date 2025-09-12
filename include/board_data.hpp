#pragma once

#include "game_data.hpp"
#include "clearing_data.hpp"
#include "forest_data.hpp"

#include <cstdint>
#include <array>
#include <string_view>
#include <expected>
#include <vector>
#include <optional>
#include <bitset>
#include <algorithm>
#include <span>
#include <variant>

namespace game_data
{
namespace board_data
{

static constexpr uint8_t kTotalBoardTypes = 4;

static constexpr std::array<std::bitset<kTotalClearings * kTotalForests>, kTotalBoardTypes> clearingForestConnections 
{[]{
    std::array<std::bitset<kTotalClearings * kTotalForests>, kTotalBoardTypes> result{};

    using enum BoardType;
    result[static_cast<size_t>(kAutumn)].set(0 * kTotalForests + 0, true);
    result[static_cast<size_t>(kAutumn)].set(0 * kTotalForests + 0, true);
    result[static_cast<size_t>(kAutumn)].set(0 * kTotalForests + 2, true);

    result[static_cast<size_t>(kAutumn)].set(1 * kTotalForests + 0, true);

    result[static_cast<size_t>(kAutumn)].set(2 * kTotalForests + 0, true);
    result[static_cast<size_t>(kAutumn)].set(2 * kTotalForests + 2, true);

    result[static_cast<size_t>(kAutumn)].set(3 * kTotalForests + 0, true);
    result[static_cast<size_t>(kAutumn)].set(3 * kTotalForests + 1, true);
    result[static_cast<size_t>(kAutumn)].set(3 * kTotalForests + 2, true);

    result[static_cast<size_t>(kAutumn)].set(4 * kTotalForests + 1, true);
    result[static_cast<size_t>(kAutumn)].set(4 * kTotalForests + 3, true);

    result[static_cast<size_t>(kAutumn)].set(5 * kTotalForests + 2, true);
    result[static_cast<size_t>(kAutumn)].set(5 * kTotalForests + 4, true);
    result[static_cast<size_t>(kAutumn)].set(5 * kTotalForests + 5, true);

    result[static_cast<size_t>(kAutumn)].set(6 * kTotalForests + 2, true);
    result[static_cast<size_t>(kAutumn)].set(6 * kTotalForests + 4, true);

    result[static_cast<size_t>(kAutumn)].set(7 * kTotalForests + 1, true);
    result[static_cast<size_t>(kAutumn)].set(7 * kTotalForests + 2, true);
    result[static_cast<size_t>(kAutumn)].set(7 * kTotalForests + 3, true);
    result[static_cast<size_t>(kAutumn)].set(7 * kTotalForests + 5, true);
    result[static_cast<size_t>(kAutumn)].set(7 * kTotalForests + 6, true);

    result[static_cast<size_t>(kAutumn)].set(8 * kTotalForests + 5, true);
    result[static_cast<size_t>(kAutumn)].set(8 * kTotalForests + 6, true);

    result[static_cast<size_t>(kAutumn)].set(9 * kTotalForests + 3, true);
    result[static_cast<size_t>(kAutumn)].set(9 * kTotalForests + 6, true);

    result[static_cast<size_t>(kAutumn)].set(10 * kTotalForests + 4, true);
    result[static_cast<size_t>(kAutumn)].set(10 * kTotalForests + 5, true);

    result[static_cast<size_t>(kAutumn)].set(11 * kTotalForests + 6, true);



    result[static_cast<size_t>(kWinter)].set(0 * kTotalForests + 0, true);
    result[static_cast<size_t>(kWinter)].set(0 * kTotalForests + 1, true);

    result[static_cast<size_t>(kWinter)].set(1 * kTotalForests + 0, true);

    result[static_cast<size_t>(kWinter)].set(2 * kTotalForests + 0, true);
    result[static_cast<size_t>(kWinter)].set(2 * kTotalForests + 2, true);

    result[static_cast<size_t>(kWinter)].set(3 * kTotalForests + 0, true);

    result[static_cast<size_t>(kWinter)].set(4 * kTotalForests + 1, true);
    result[static_cast<size_t>(kWinter)].set(4 * kTotalForests + 4, true);

    result[static_cast<size_t>(kWinter)].set(5 * kTotalForests + 0, true);
    result[static_cast<size_t>(kWinter)].set(5 * kTotalForests + 2, true);
    result[static_cast<size_t>(kWinter)].set(5 * kTotalForests + 3, true);
    result[static_cast<size_t>(kWinter)].set(5 * kTotalForests + 5, true);
    result[static_cast<size_t>(kWinter)].set(5 * kTotalForests + 6, true);

    result[static_cast<size_t>(kWinter)].set(6 * kTotalForests + 0, true);
    result[static_cast<size_t>(kWinter)].set(6 * kTotalForests + 1, true);
    result[static_cast<size_t>(kWinter)].set(6 * kTotalForests + 3, true);
    result[static_cast<size_t>(kWinter)].set(6 * kTotalForests + 4, true);
    result[static_cast<size_t>(kWinter)].set(6 * kTotalForests + 7, true);

    result[static_cast<size_t>(kWinter)].set(7 * kTotalForests + 2, true);
    result[static_cast<size_t>(kWinter)].set(7 * kTotalForests + 6, true);

    result[static_cast<size_t>(kWinter)].set(8 * kTotalForests + 3, true);
    result[static_cast<size_t>(kWinter)].set(8 * kTotalForests + 5, true);

    result[static_cast<size_t>(kWinter)].set(9 * kTotalForests + 4, true);
    result[static_cast<size_t>(kWinter)].set(9 * kTotalForests + 7, true);

    result[static_cast<size_t>(kWinter)].set(10 * kTotalForests + 5, true);
    result[static_cast<size_t>(kWinter)].set(10 * kTotalForests + 6, true);

    result[static_cast<size_t>(kWinter)].set(11 * kTotalForests + 3, true);
    result[static_cast<size_t>(kWinter)].set(11 * kTotalForests + 7, true);    



    result[static_cast<size_t>(kLake)].set(0 * kTotalForests + 0, true);
    result[static_cast<size_t>(kLake)].set(0 * kTotalForests + 1, true);
    result[static_cast<size_t>(kLake)].set(0 * kTotalForests + 2, true);

    result[static_cast<size_t>(kLake)].set(1 * kTotalForests + 0, true);
    result[static_cast<size_t>(kLake)].set(1 * kTotalForests + 4, true);

    result[static_cast<size_t>(kLake)].set(2 * kTotalForests + 1, true);
    result[static_cast<size_t>(kLake)].set(2 * kTotalForests + 3, true);

    result[static_cast<size_t>(kLake)].set(3 * kTotalForests + 3, true);

    result[static_cast<size_t>(kLake)].set(4 * kTotalForests + 0, true);
    result[static_cast<size_t>(kLake)].set(4 * kTotalForests + 2, true);
    result[static_cast<size_t>(kLake)].set(4 * kTotalForests + 4, true);
    result[static_cast<size_t>(kLake)].set(4 * kTotalForests + 5, true);

    result[static_cast<size_t>(kLake)].set(5 * kTotalForests + 1, true);
    result[static_cast<size_t>(kLake)].set(5 * kTotalForests + 2, true);
    result[static_cast<size_t>(kLake)].set(5 * kTotalForests + 3, true);
    result[static_cast<size_t>(kLake)].set(5 * kTotalForests + 6, true);

    result[static_cast<size_t>(kLake)].set(6 * kTotalForests + 4, true);
    result[static_cast<size_t>(kLake)].set(6 * kTotalForests + 5, true);

    result[static_cast<size_t>(kLake)].set(7 * kTotalForests + 3, true);
    result[static_cast<size_t>(kLake)].set(7 * kTotalForests + 6, true);

    result[static_cast<size_t>(kLake)].set(8 * kTotalForests + 5, true);
    result[static_cast<size_t>(kLake)].set(8 * kTotalForests + 7, true);
    result[static_cast<size_t>(kLake)].set(8 * kTotalForests + 8, true);

    result[static_cast<size_t>(kLake)].set(9 * kTotalForests + 5, true);
    result[static_cast<size_t>(kLake)].set(9 * kTotalForests + 8, true);

    result[static_cast<size_t>(kLake)].set(10 * kTotalForests + 6, true);
    result[static_cast<size_t>(kLake)].set(10 * kTotalForests + 7, true);

    result[static_cast<size_t>(kLake)].set(11 * kTotalForests + 7, true);
    result[static_cast<size_t>(kLake)].set(11 * kTotalForests + 8, true);



    result[static_cast<size_t>(kMountain)].set(0 * kTotalForests + 0, true);
    result[static_cast<size_t>(kMountain)].set(0 * kTotalForests + 1, true);
    result[static_cast<size_t>(kMountain)].set(0 * kTotalForests + 2, true);

    result[static_cast<size_t>(kMountain)].set(1 * kTotalForests + 3, true);

    result[static_cast<size_t>(kMountain)].set(2 * kTotalForests + 0, true);
    result[static_cast<size_t>(kMountain)].set(2 * kTotalForests + 4, true);

    result[static_cast<size_t>(kMountain)].set(3 * kTotalForests + 1, true);
    result[static_cast<size_t>(kMountain)].set(3 * kTotalForests + 3, true);
    result[static_cast<size_t>(kMountain)].set(3 * kTotalForests + 5, true);
    result[static_cast<size_t>(kMountain)].set(3 * kTotalForests + 6, true);

    result[static_cast<size_t>(kMountain)].set(4 * kTotalForests + 1, true);
    result[static_cast<size_t>(kMountain)].set(4 * kTotalForests + 2, true);
    result[static_cast<size_t>(kMountain)].set(4 * kTotalForests + 5, true);
    result[static_cast<size_t>(kMountain)].set(4 * kTotalForests + 7, true);

    result[static_cast<size_t>(kMountain)].set(5 * kTotalForests + 4, true);
    result[static_cast<size_t>(kMountain)].set(5 * kTotalForests + 8, true);

    result[static_cast<size_t>(kMountain)].set(6 * kTotalForests + 3, true);
    result[static_cast<size_t>(kMountain)].set(6 * kTotalForests + 6, true);

    result[static_cast<size_t>(kMountain)].set(7 * kTotalForests + 0, true);
    result[static_cast<size_t>(kMountain)].set(7 * kTotalForests + 2, true);
    result[static_cast<size_t>(kMountain)].set(7 * kTotalForests + 4, true);
    result[static_cast<size_t>(kMountain)].set(7 * kTotalForests + 7, true);
    result[static_cast<size_t>(kMountain)].set(7 * kTotalForests + 8, true);
    result[static_cast<size_t>(kMountain)].set(7 * kTotalForests + 9, true);

    result[static_cast<size_t>(kMountain)].set(8 * kTotalForests + 5, true);
    result[static_cast<size_t>(kMountain)].set(8 * kTotalForests + 6, true);
    result[static_cast<size_t>(kMountain)].set(8 * kTotalForests + 7, true);
    result[static_cast<size_t>(kMountain)].set(8 * kTotalForests + 9, true);

    result[static_cast<size_t>(kMountain)].set(9 * kTotalForests + 8, true);
    result[static_cast<size_t>(kMountain)].set(9 * kTotalForests + 9, true);

    result[static_cast<size_t>(kMountain)].set(10 * kTotalForests + 6, true);

    result[static_cast<size_t>(kMountain)].set(11 * kTotalForests + 10, true);
    

    return result;
}()};

static constexpr std::array<std::bitset<kTotalClearings * kTotalForests>, kTotalBoardTypes> forestClearingConnections{[]{
    std::array<std::bitset<kTotalClearings * kTotalForests>, kTotalBoardTypes> result{};

    for (uint8_t board = 0; board < kTotalBoardTypes; ++board) {
        for (uint8_t clearing = 0; clearing < kTotalClearings; ++clearing) {
            for (uint8_t forest = 0; forest < kTotalForests; ++forest) {    
                if (clearingForestConnections[board][clearing * kTotalForests + forest]) {
                    result[board].set(forest * kTotalClearings + clearing, true);
                }
            }
        }
    }
    return result;
}()};

enum class ClearingClearingConnectionType : uint8_t
{
    kNoConnection,
    kLand,
    kWater,
};

static constexpr std::array<std::array<std::array<ClearingClearingConnectionType, kTotalClearings>, kTotalClearings>, kTotalBoardTypes> clearingClearingConnections{[]{
    using enum ClearingClearingConnectionType;

    static_assert(static_cast<std::underlying_type_t<ClearingClearingConnectionType>>(kNoConnection) == 0, "kNoConnection must equal 0");

    std::array<std::array<std::array<ClearingClearingConnectionType, kTotalClearings>, kTotalClearings>, kTotalBoardTypes> result{};

    using enum BoardType;
    result[static_cast<size_t>(kAutumn)][0][1] = kLand;
    result[static_cast<size_t>(kAutumn)][0][3] = kLand;
    result[static_cast<size_t>(kAutumn)][0][4] = kLand;

    result[static_cast<size_t>(kAutumn)][1][0] = kLand;
    result[static_cast<size_t>(kAutumn)][1][2] = kLand;
    result[static_cast<size_t>(kAutumn)][1][3] = kWater;

    result[static_cast<size_t>(kAutumn)][2][1] = kLand;
    result[static_cast<size_t>(kAutumn)][2][3] = kLand;
    result[static_cast<size_t>(kAutumn)][2][6] = kLand;

    result[static_cast<size_t>(kAutumn)][3][0] = kLand;
    result[static_cast<size_t>(kAutumn)][3][1] = kWater;
    result[static_cast<size_t>(kAutumn)][3][2] = kLand;
    result[static_cast<size_t>(kAutumn)][3][5] = kWater;
    result[static_cast<size_t>(kAutumn)][3][7] = kLand;

    result[static_cast<size_t>(kAutumn)][4][0] = kLand;
    result[static_cast<size_t>(kAutumn)][4][7] = kLand;
    result[static_cast<size_t>(kAutumn)][4][9] = kLand;

    result[static_cast<size_t>(kAutumn)][5][3] = kWater;
    result[static_cast<size_t>(kAutumn)][5][6] = kLand;
    result[static_cast<size_t>(kAutumn)][5][7] = kLand;
    result[static_cast<size_t>(kAutumn)][5][8] = kWater;
    result[static_cast<size_t>(kAutumn)][5][10] = kLand;

    result[static_cast<size_t>(kAutumn)][6][2] = kLand;
    result[static_cast<size_t>(kAutumn)][6][5] = kLand;
    result[static_cast<size_t>(kAutumn)][6][10] = kLand;

    result[static_cast<size_t>(kAutumn)][7][3] = kLand;
    result[static_cast<size_t>(kAutumn)][7][4] = kLand;
    result[static_cast<size_t>(kAutumn)][7][5] = kLand;
    result[static_cast<size_t>(kAutumn)][7][8] = kLand;
    result[static_cast<size_t>(kAutumn)][7][9] = kLand;

    result[static_cast<size_t>(kAutumn)][8][5] = kWater;
    result[static_cast<size_t>(kAutumn)][8][7] = kLand;
    result[static_cast<size_t>(kAutumn)][8][9] = kWater;
    result[static_cast<size_t>(kAutumn)][8][10] = kLand;
    result[static_cast<size_t>(kAutumn)][8][11] = kLand;

    result[static_cast<size_t>(kAutumn)][9][4] = kLand;
    result[static_cast<size_t>(kAutumn)][9][7] = kLand;
    result[static_cast<size_t>(kAutumn)][9][8] = kWater;
    result[static_cast<size_t>(kAutumn)][9][11] = kLand;

    result[static_cast<size_t>(kAutumn)][10][5] = kLand;
    result[static_cast<size_t>(kAutumn)][10][6] = kLand;
    result[static_cast<size_t>(kAutumn)][10][8] = kLand;

    result[static_cast<size_t>(kAutumn)][11][8] = kLand;
    result[static_cast<size_t>(kAutumn)][11][9] = kLand;
    


    result[static_cast<size_t>(kWinter)][0][1] = kLand;
    result[static_cast<size_t>(kWinter)][0][4] = kLand;
    result[static_cast<size_t>(kWinter)][0][6] = kLand;

    result[static_cast<size_t>(kWinter)][1][0] = kLand;
    result[static_cast<size_t>(kWinter)][1][3] = kLand;

    result[static_cast<size_t>(kWinter)][2][3] = kLand;
    result[static_cast<size_t>(kWinter)][2][5] = kLand;
    result[static_cast<size_t>(kWinter)][2][7] = kLand;

    result[static_cast<size_t>(kWinter)][3][1] = kLand;
    result[static_cast<size_t>(kWinter)][3][2] = kLand;

    result[static_cast<size_t>(kWinter)][4][0] = kLand;
    result[static_cast<size_t>(kWinter)][4][6] = kWater;
    result[static_cast<size_t>(kWinter)][4][9] = kLand;

    result[static_cast<size_t>(kWinter)][5][2] = kLand;
    result[static_cast<size_t>(kWinter)][5][6] = kWater;
    result[static_cast<size_t>(kWinter)][5][7] = kWater;
    result[static_cast<size_t>(kWinter)][5][8] = kLand;
    result[static_cast<size_t>(kWinter)][5][10] = kLand;

    result[static_cast<size_t>(kWinter)][6][0] = kLand;
    result[static_cast<size_t>(kWinter)][6][4] = kWater;
    result[static_cast<size_t>(kWinter)][6][5] = kWater;
    result[static_cast<size_t>(kWinter)][6][9] = kLand;
    result[static_cast<size_t>(kWinter)][6][11] = kLand;

    result[static_cast<size_t>(kWinter)][7][2] = kLand;
    result[static_cast<size_t>(kWinter)][7][5] = kWater;
    result[static_cast<size_t>(kWinter)][7][10] = kLand;

    result[static_cast<size_t>(kWinter)][8][5] = kLand;
    result[static_cast<size_t>(kWinter)][8][10] = kLand;
    result[static_cast<size_t>(kWinter)][8][11] = kLand;

    result[static_cast<size_t>(kWinter)][9][4] = kLand;
    result[static_cast<size_t>(kWinter)][9][6] = kLand;
    result[static_cast<size_t>(kWinter)][9][11] = kLand;

    result[static_cast<size_t>(kWinter)][10][5] = kLand;
    result[static_cast<size_t>(kWinter)][10][7] = kLand;
    result[static_cast<size_t>(kWinter)][10][8] = kLand;

    result[static_cast<size_t>(kWinter)][11][8] = kLand;
    result[static_cast<size_t>(kWinter)][11][9] = kLand;



    result[static_cast<size_t>(kLake)][0][1] = kLand;
    result[static_cast<size_t>(kLake)][0][2] = kLand;
    result[static_cast<size_t>(kLake)][0][4] = kLand;
    result[static_cast<size_t>(kLake)][0][5] = kLand;

    result[static_cast<size_t>(kLake)][1][0] = kLand;
    result[static_cast<size_t>(kLake)][1][4] = kLand;
    result[static_cast<size_t>(kLake)][1][6] = kLand;

    result[static_cast<size_t>(kLake)][2][0] = kLand;
    result[static_cast<size_t>(kLake)][2][3] = kLand;
    result[static_cast<size_t>(kLake)][2][5] = kLand;

    result[static_cast<size_t>(kLake)][3][2] = kLand;
    result[static_cast<size_t>(kLake)][3][7] = kLand;

    result[static_cast<size_t>(kLake)][4][0] = kLand;
    result[static_cast<size_t>(kLake)][4][1] = kLand;
    result[static_cast<size_t>(kLake)][4][5] = kWater;
    result[static_cast<size_t>(kLake)][4][6] = kLand;
    result[static_cast<size_t>(kLake)][4][8] = kWater;
    result[static_cast<size_t>(kLake)][4][10] = kWater;

    result[static_cast<size_t>(kLake)][5][0] = kLand;
    result[static_cast<size_t>(kLake)][5][2] = kLand;
    result[static_cast<size_t>(kLake)][5][4] = kWater;
    result[static_cast<size_t>(kLake)][5][7] = kLand;
    result[static_cast<size_t>(kLake)][5][8] = kWater;
    result[static_cast<size_t>(kLake)][5][10] = kWater;

    result[static_cast<size_t>(kLake)][6][1] = kLand;
    result[static_cast<size_t>(kLake)][6][4] = kLand;
    result[static_cast<size_t>(kLake)][6][9] = kLand;

    result[static_cast<size_t>(kLake)][7][3] = kLand;
    result[static_cast<size_t>(kLake)][7][5] = kLand;
    result[static_cast<size_t>(kLake)][7][10] = kLand;

    result[static_cast<size_t>(kLake)][8][4] = kWater;
    result[static_cast<size_t>(kLake)][8][5] = kWater;
    result[static_cast<size_t>(kLake)][8][9] = kLand;
    result[static_cast<size_t>(kLake)][8][10] = kWater;
    result[static_cast<size_t>(kLake)][8][11] = kLand;

    result[static_cast<size_t>(kLake)][9][6] = kLand;
    result[static_cast<size_t>(kLake)][9][8] = kLand;
    result[static_cast<size_t>(kLake)][9][11] = kLand;

    result[static_cast<size_t>(kLake)][10][4] = kWater;
    result[static_cast<size_t>(kLake)][10][5] = kWater;
    result[static_cast<size_t>(kLake)][10][7] = kLand;
    result[static_cast<size_t>(kLake)][10][8] = kWater;
    result[static_cast<size_t>(kLake)][10][11] = kLand;

    result[static_cast<size_t>(kLake)][11][8] = kLand;
    result[static_cast<size_t>(kLake)][11][9] = kLand;
    result[static_cast<size_t>(kLake)][11][10] = kLand;



    result[static_cast<size_t>(kMountain)][0][4] = kLand;
    result[static_cast<size_t>(kMountain)][0][7] = kLand;

    result[static_cast<size_t>(kMountain)][1][3] = kLand;
    result[static_cast<size_t>(kMountain)][1][6] = kLand;

    result[static_cast<size_t>(kMountain)][2][4] = kWater;
    result[static_cast<size_t>(kMountain)][2][5] = kLand;
    result[static_cast<size_t>(kMountain)][2][7] = kLand;

    result[static_cast<size_t>(kMountain)][3][1] = kLand;
    result[static_cast<size_t>(kMountain)][3][4] = kLand;

    result[static_cast<size_t>(kMountain)][3][8] = kLand;

    result[static_cast<size_t>(kMountain)][4][0] = kLand;
    result[static_cast<size_t>(kMountain)][4][2] = kWater;
    result[static_cast<size_t>(kMountain)][4][3] = kLand;
    result[static_cast<size_t>(kMountain)][4][7] = kLand;
    result[static_cast<size_t>(kMountain)][4][8] = kLand;
    result[static_cast<size_t>(kMountain)][4][10] = kWater;

    result[static_cast<size_t>(kMountain)][5][2] = kLand;
    result[static_cast<size_t>(kMountain)][5][9] = kLand;

    result[static_cast<size_t>(kMountain)][6][1] = kLand;
    result[static_cast<size_t>(kMountain)][6][10] = kLand;

    result[static_cast<size_t>(kMountain)][7][0] = kLand;
    result[static_cast<size_t>(kMountain)][7][2] = kLand;
    result[static_cast<size_t>(kMountain)][7][4] = kLand;
    result[static_cast<size_t>(kMountain)][7][9] = kLand;

    result[static_cast<size_t>(kMountain)][8][3] = kLand;
    result[static_cast<size_t>(kMountain)][8][4] = kLand;
    result[static_cast<size_t>(kMountain)][8][10] = kLand;
    result[static_cast<size_t>(kMountain)][8][11] = kLand;

    result[static_cast<size_t>(kMountain)][9][5] = kLand;
    result[static_cast<size_t>(kMountain)][9][7] = kLand;

    result[static_cast<size_t>(kMountain)][10][4] = kWater;
    result[static_cast<size_t>(kMountain)][10][6] = kLand;
    result[static_cast<size_t>(kMountain)][10][8] = kLand;

    result[static_cast<size_t>(kMountain)][11][8] = kLand;

    return result;
}()};

static constexpr std::array<std::array<uint8_t, kTotalClearings>, kTotalBoardTypes> startingBuildingSlotCounts{{
    // Autumn
    {1, 2, 2, 2, 1, 3, 2, 2, 2, 1, 1, 2},

    // Winter
    {1, 2, 1, 2, 1, 3, 3, 1, 2, 2, 2, 2},

    // Lake
    {1, 1, 2, 1, 3, 3, 1, 3, 3, 1, 2, 1},

    // Mountain
    {1, 2, 2, 3, 2, 1, 1, 3, 3, 2, 2, 1}
}};

consteval inline void add_ruin(
    std::array<std::bitset<kTotalClearings>, kTotalBoardTypes>& result,
    const BoardType boardType,
    const uint8_t clearingIndex
) {
    result[static_cast<size_t>(boardType)].set(clearingIndex, true);
}

static constexpr std::array<std::bitset<kTotalClearings>, kTotalBoardTypes> startingRuins{[]{
    std::array<std::bitset<kTotalClearings>, kTotalBoardTypes> result{};

    using enum BoardType;
    result[static_cast<size_t>(kAutumn)].set(3, true);
    result[static_cast<size_t>(kAutumn)].set(5, true);
    result[static_cast<size_t>(kAutumn)].set(6, true);
    result[static_cast<size_t>(kAutumn)].set(7, true);
    
    result[static_cast<size_t>(kWinter)].set(5, true);
    result[static_cast<size_t>(kWinter)].set(6, true);
    result[static_cast<size_t>(kWinter)].set(8, true);
    result[static_cast<size_t>(kWinter)].set(11, true);

    result[static_cast<size_t>(kLake)].set(4, true);
    result[static_cast<size_t>(kLake)].set(5, true);
    result[static_cast<size_t>(kLake)].set(7, true);
    result[static_cast<size_t>(kLake)].set(8, true);

    result[static_cast<size_t>(kMountain)].set(3, true);
    result[static_cast<size_t>(kMountain)].set(4, true);
    result[static_cast<size_t>(kMountain)].set(7, true);
    result[static_cast<size_t>(kMountain)].set(8, true);

    return result;
}()};

static constexpr std::array<std::array<clearing_data::landmark_data::Landmark, kTotalClearings>, kTotalBoardTypes> startingLandmarks{[]{
    using Landmark = clearing_data::landmark_data::Landmark;
    using enum Landmark;

    static_assert(static_cast<std::underlying_type_t<Landmark>>(kNone) == 0);
    std::array<std::array<Landmark, kTotalClearings>, kTotalBoardTypes> result{};

    using enum BoardType;

    result[static_cast<size_t>(kLake)][10] = kFerry;
    result[static_cast<size_t>(kMountain)][4] = kTower;

    return result;
}()};

static constexpr std::array<std::array<clearing_data::ClearingType, kTotalClearings>, kTotalBoardTypes> clearingTypes{[]{
    using ClearingType = clearing_data::ClearingType;
    using enum ClearingType;

    static_assert(static_cast<std::underlying_type_t<ClearingType>>(kRandom) == 0);

    std::array<std::array<ClearingType, kTotalClearings>, kTotalBoardTypes> result{};

    using enum BoardType;
    result[static_cast<size_t>(kAutumn)] = 
        {kFox, kRabbit, kMouse, kRabbit, kMouse, kMouse, kFox, kFox, kMouse, kRabbit, kRabbit, kFox};

    return result;
}()};

struct ConnectionError {
    enum class Code : uint8_t {
        kNotEnoughDataRead,
        kNotEnoughDataWrite,
        kInvalidConnectionType,
        kIndexExceededNodeCount,
        kDuplicateIndices
    } code;

    static constexpr std::array<std::string_view, 6> kMessages = {
        "Not enough data to read requested bits",
        "Not enough data to write requested bits",
        "Invalid connection type",
        "Index exceeded node count",
        "Duplicate indices are not allowed",
        "Unknown Error"
    };

    [[nodiscard]] static std::string_view to_string(Code code) {
        uint8_t idx = static_cast<uint8_t>(code);
        if (idx < kMessages.size()) return kMessages[idx];
        return kMessages.back();
    }

    [[nodiscard]] std::string_view message() const { return to_string(code); }
};

struct ClearingConnection
{
    uint8_t clearingIndex;
    ClearingClearingConnectionType connectionType;
};

enum class BasicConnectionType : uint8_t {
    kClearingForest,
    kForestClearing
};

template <
    BoardType boardType,
    BasicConnectionType connectionType
>
[[nodiscard]] inline constexpr 
std::expected<bool, ConnectionError> get_basic_connection(
    const uint8_t originIndex,
    const uint8_t destinationIndex
) {
    [[unlikely]] if (originIndex == destinationIndex)
        return std::unexpected(ConnectionError{ConnectionError::Code::kDuplicateIndices});

    using enum BasicConnectionType;
    if constexpr (connectionType == kClearingForest) {
        [[unlikely]] if (originIndex >= kTotalForests || destinationIndex >= kTotalForests)
            return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

        return clearingForestConnections[static_cast<size_t>(boardType)][originIndex * kTotalForests + destinationIndex];
    } else if constexpr (connectionType == kForestClearing) {
        [[unlikely]] if (originIndex >= kTotalClearings || destinationIndex >= kTotalClearings)
            return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

        return forestClearingConnections[static_cast<size_t>(boardType)][originIndex * kTotalForests + destinationIndex];
    } else {
        return std::unexpected(ConnectionError{ConnectionError::Code::kInvalidConnectionType});
    }
}

template <
    BoardType boardType,
    BasicConnectionType connectionType,
    uint8_t originIndex,
    uint8_t destinationIndex
>
[[nodiscard]] inline consteval
bool get_basic_connection() {
    static_assert(originIndex != destinationIndex, "Duplicate indices are not allowed");

    using enum BasicConnectionType;
    static_assert(connectionType == kClearingForest || connectionType == kForestClearing, "Invalid connection type");

    if constexpr (connectionType == kClearingForest) {
        static_assert(originIndex < kTotalForests || destinationIndex < kTotalForests, "Index exceeded node count");

        return clearingForestConnections[static_cast<size_t>(boardType)][originIndex * kTotalForests + destinationIndex];
    } else if constexpr (connectionType == kForestClearing) {
        static_assert(originIndex < kTotalClearings || destinationIndex < kTotalClearings, "Index exceeded node count");

        return forestClearingConnections[static_cast<size_t>(boardType)][originIndex * kTotalForests + destinationIndex];
    }
}

template <BasicConnectionType connectionType>
[[nodiscard]] inline consteval uint8_t get_max_connections_from_type() {
    using enum BasicConnectionType;
    static_assert(connectionType == kClearingForest || connectionType == kForestClearing, "Invalid connection type");

    if constexpr (connectionType == kClearingForest) {
        return kTotalForests;
    } else if constexpr (connectionType == kForestClearing) {
        return kTotalClearings;
    }
}

template <
    BoardType boardType,
    BasicConnectionType connectionType
>
[[nodiscard]] inline constexpr
std::expected<std::bitset<get_max_connections_from_type<connectionType>()>, ConnectionError> get_basic_connections(const uint8_t desiredIndex)
{
    using enum BasicConnectionType;
    static_assert(connectionType == kClearingForest || connectionType == kForestClearing, "Invalid connection type");

    if constexpr (connectionType == kClearingForest) {
        [[unlikely]] if (desiredIndex >= kTotalForests)
            return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

        return std::bitset<kTotalForests>{(clearingForestConnections[static_cast<size_t>(boardType)] >> (desiredIndex * kTotalForests)).to_ulong()};
    } else if constexpr (connectionType == kForestClearing) {
        [[unlikely]] if (desiredIndex >= kTotalClearings)
            return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

        return std::bitset<kTotalClearings>{(forestClearingConnections[static_cast<size_t>(boardType)] >> (desiredIndex * kTotalClearings)).to_ulong()};
    }
}

template <
    BoardType boardType,
    BasicConnectionType connectionType,
    uint8_t desiredIndex
>
[[nodiscard]] inline consteval
std::expected<std::bitset<get_max_connections_from_type<connectionType>()>, ConnectionError> get_basic_connections()
{
    using enum BasicConnectionType;
    static_assert(connectionType == kClearingForest || connectionType == kForestClearing, "Invalid connection type");

    if constexpr (connectionType == kClearingForest) {
        static_assert(desiredIndex < kTotalForests, "Index exceeded node count");
        return std::bitset<kTotalForests>{(clearingForestConnections[static_cast<size_t>(boardType)] >> (desiredIndex * kTotalForests)).to_ulong()};
    } else if constexpr (connectionType == kForestClearing) {
        static_assert(desiredIndex < kTotalClearings, "Index exceeded node count");
        return std::bitset<kTotalClearings>{(forestClearingConnections[static_cast<size_t>(boardType)] >> (desiredIndex * kTotalClearings)).to_ulong()};
    }
}

template <BoardType boardType>
[[nodiscard]] inline constexpr
std::expected<ClearingClearingConnectionType, ConnectionError> get_clearing_clearing_connection(
    const uint8_t originIndex,
    const uint8_t destinationIndex
) {
    [[unlikely]] if (originIndex == destinationIndex)
        return std::unexpected(ConnectionError{ConnectionError::Code::kDuplicateIndices});

    [[unlikely]] if (originIndex >= kTotalClearings || destinationIndex >= kTotalClearings)
        return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

    return std::expected<ClearingClearingConnectionType, ConnectionError>{clearingClearingConnections[static_cast<size_t>(boardType)][originIndex][destinationIndex]};
}

template <
    BoardType boardType,
    uint8_t originIndex,
    uint8_t destinationIndex
>
[[nodiscard]] inline consteval
ClearingClearingConnectionType get_clearing_clearing_connection() {
    static_assert(originIndex != destinationIndex, "Duplicate indices are not allowed");
    static_assert(originIndex < kTotalClearings || destinationIndex < kTotalClearings, "Index exceeded node count");

    return clearingClearingConnections[static_cast<size_t>(boardType)][originIndex][destinationIndex];
}

template <BoardType boardType>
[[nodiscard]] inline constexpr
std::expected<std::array<ClearingClearingConnectionType, kTotalClearings>, ConnectionError> 
get_clearing_clearing_connections(const uint8_t desiredIndex) {
    [[unlikely]] if (desiredIndex >= kTotalClearings)
        return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

    return clearingClearingConnections[static_cast<size_t>(boardType)][desiredIndex];
}

template <
    BoardType boardType,
    uint8_t desiredIndex
>
[[nodiscard]] inline consteval
std::array<ClearingClearingConnectionType, kTotalClearings> get_clearing_clearing_connections() {
    static_assert(desiredIndex < kTotalClearings, "Index exceeded node count");
    return clearingClearingConnections[static_cast<size_t>(boardType)][desiredIndex];
}

struct ClearingClearingConnectionList {
    std::array<uint8_t, kTotalClearings> nodes;
    uint8_t count = 0;

    [[nodiscard]] inline constexpr uint8_t size() const { return count; }
    [[nodiscard]] inline constexpr const uint8_t* data() const { return nodes.data(); }
    [[nodiscard]] inline constexpr uint8_t* data() { return nodes.data(); }

    // implicit conversion to read-only span
    constexpr operator std::span<const uint8_t>() const {
        return { nodes.data(), count };
    }
};

template <
    BoardType boardType,
    ClearingClearingConnectionType connectionType
>
[[nodiscard]] inline constexpr
std::expected<ClearingClearingConnectionList, ConnectionError> get_clearing_clearing_connections_filtered(const uint8_t desiredIndex) {
    [[unlikely]] if (desiredIndex >= kTotalClearings) 
        return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

    ClearingClearingConnectionList matchingConnectionList;
    auto unroll = [&desiredIndex, &matchingConnectionList]<size_t... i>(std::index_sequence<i...>)
    {
        (([&desiredIndex, &matchingConnectionList] {
            if (
                i != desiredIndex &&
                clearingClearingConnections[static_cast<size_t>(boardType)][desiredIndex][i] == connectionType
            )
            {
                ++matchingConnectionList.count;
                matchingConnectionList.nodes[matchingConnectionList.count] = i;
            }
        }()), ...);
    }(std::make_index_sequence<kTotalClearings>{});

    return matchingConnectionList;
}

template  <
    BoardType boardType,
    ClearingClearingConnectionType connectionType,
    uint8_t desiredIndex
>
[[nodiscard]] inline consteval
ClearingClearingConnectionList get_clearing_clearing_connections_filtered() {
    static_assert(desiredIndex < kTotalClearings, "Index exceeded node count");

    ClearingClearingConnectionList matchingConnectionList;
    auto unroll = [&matchingConnectionList]<size_t... i>(std::index_sequence<i...>)
    {
        (([&matchingConnectionList] {
            if constexpr (i != desiredIndex)
            {
                if constexpr (clearingClearingConnections[static_cast<size_t>(boardType)][desiredIndex][i] == connectionType) {
                    ++matchingConnectionList.count;
                    matchingConnectionList.nodes[matchingConnectionList.count] = i;
                }
            }
        }()), ...);
    }(std::make_index_sequence<kTotalClearings>{});

    return matchingConnectionList;
}

template<BoardType boardType>
class Board
{
private:
    r123::Threefry2x32_R<12>::ctr_type ctr;
    const r123::Threefry2x32_R<12>::key_type &key;
public:
    std::array<forest_data::Forest, kTotalForests> forests;

    decltype([]<std::size_t... I>(r123::Threefry2x32_R<12>::ctr_type& ctr, const r123::Threefry2x32_R<12>::key_type& key, std::index_sequence<I...>) {
        return std::make_tuple(
            clearing_data::Clearing<
                clearingTypes[static_cast<size_t>(boardType)][I],
                startingBuildingSlotCounts[static_cast<size_t>(boardType)][I],
                startingRuins[static_cast<size_t>(boardType)][I]
            >(ctr, key)...
        );
    }(
        ctr, key, std::make_index_sequence<kTotalClearings>{}
    )) clearings;

    consteval Board(r123::Threefry2x32_R<12>::ctr_type &ctr, const r123::Threefry2x32_R<12>::key_type &key) : ctr(ctr), key(key) {};

private:
    static constexpr uint8_t kClearingClearingConnectionBits = 2;
    static constexpr uint16_t kClearingClearingsConnectionsBits = kTotalClearings * kTotalClearings * kClearingClearingConnectionBits;

    static constexpr uint8_t kClearingForestConnectionBits = 1;
    static constexpr uint8_t kClearingForestConnectionsBits = kTotalClearings * kTotalForests * kClearingForestConnectionBits;

    static constexpr uint8_t kClearingClearingsConnectionsOffset = 0;
    static constexpr uint16_t kClearingForestConnectionsOffset = kClearingClearingsConnectionsOffset + kClearingClearingsConnectionsBits;
};

template<>
class Board<BoardType::kMountain> {
    static constexpr std::bitset<kTotalClearings * kTotalClearings> initialBlockedConnections{[]{
        std::bitset<kTotalClearings * kTotalClearings> result{};
        result.set(0 * kTotalClearings + 2, true);
        result.set(0 * kTotalClearings + 3, true);
        result.set(2 * kTotalClearings + 0, true);
        result.set(3 * kTotalClearings + 0, true);
        result.set(3 * kTotalClearings + 6, true);
        result.set(5 * kTotalClearings + 7, true);
        result.set(6 * kTotalClearings + 3, true);
        result.set(7 * kTotalClearings + 5, true);
        result.set(7 * kTotalClearings + 8, true);
        result.set(8 * kTotalClearings + 7, true);
        result.set(9 * kTotalClearings + 11, true);
        result.set(11 * kTotalClearings + 9, true);

        return result;
    }()};

    std::bitset<kTotalClearings * kTotalClearings> blockedConnections = initialBlockedConnections;

    [[nodiscard]] inline std::expected<bool, ConnectionError> is_connection_blocked(uint8_t originIndex, uint8_t destinationIndex) const
    {
        [[unlikely]] if (originIndex == destinationIndex)
            return std::unexpected(ConnectionError{ConnectionError::Code::kDuplicateIndices});

        [[unlikely]] if (originIndex >= kTotalClearings || destinationIndex >= kTotalClearings)
            return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

        return blockedConnections[originIndex * kTotalClearings + destinationIndex];
    }

    template <uint8_t originIndex, uint8_t destinationIndex>
    [[nodiscard]] consteval inline bool is_connection_blocked() const
    {
        static_assert(originIndex != destinationIndex, "Duplicate indices are not allowed");
        static_assert(originIndex < kTotalClearings && destinationIndex < kTotalClearings, "Index exceeded node count");

        return blockedConnections[originIndex * kTotalClearings + destinationIndex];
    }

    inline std::expected<void, ConnectionError> set_is_connection_blocked(uint8_t originIndex, uint8_t destinationIndex, bool newStatus)
    {
        [[unlikely]] if (originIndex == destinationIndex)
            return std::unexpected(ConnectionError{ConnectionError::Code::kDuplicateIndices});

        [[unlikely]] if (originIndex >= kTotalClearings || destinationIndex >= kTotalClearings)
            return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

        blockedConnections[originIndex * kTotalClearings + destinationIndex] = newStatus;
        return {};
    }

    template <uint8_t originIndex, uint8_t destinationIndex>
    inline void constexpr set_is_connection_blocked(bool newStatus)
    {
        static_assert(originIndex != destinationIndex, "Duplicate indices are not allowed");
        static_assert(originIndex < kTotalClearings && destinationIndex < kTotalClearings, "Index exceeded node count");

        blockedConnections[originIndex * kTotalClearings + destinationIndex] = newStatus;
    }
    
    struct BlockedConnectionList {
        std::array<uint8_t, kTotalClearings> nodes;
        uint8_t count = 0;

        [[nodiscard]] inline constexpr uint8_t size() const { return count; }
        [[nodiscard]] inline constexpr const uint8_t* data() const { return nodes.data(); }
        [[nodiscard]] inline constexpr uint8_t* data() { return nodes.data(); }

        // implicit conversion to read-only span
        constexpr operator std::span<const uint8_t>() const {
            return { nodes.data(), count };
        }       
    };

    enum class IsBlocked : bool {
        kUnblocked,
        kBlocked
    };

    template <IsBlocked desiredType>
    [[nodiscard]] inline std::expected<BlockedConnectionList, ConnectionError> get_blocked_connections_with_status(uint8_t desiredIndex) const
    {
        [[unlikely]] if (desiredIndex >= kTotalClearings)
            return std::unexpected(ConnectionError{ConnectionError::Code::kIndexExceededNodeCount});

        BlockedConnectionList matchingConnectionList;
        auto unroll[&desiredIndex, &matchingConnectionList]<size_t... i>(std::index_sequenceM<i...>)
        {
            (([&desiredIndex, &matchingConnectionList] {
                if (
                    i != desiredIndex &&
                    clearingClearingConnections[static_cast<size_t>(boardType)][desiredIndex][i] == desiredType
                )
                {
                    ++matchingConnectionList.count;
                    matchingConnectionList.nodes[matchingConnectionList.count] = i;
                }
            }()), ...);
        }(std::make_index_sequence<kTotalClearings>{});

        return matchingConnectionList;
    }
};
} // board_data
} // game_data