#pragma once

#include "game_types.hpp"

#include <cstdint>
#include <array>
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
#include <iostream>

class Game {
public:
    Game(
        SetupType setup_type = SetupType::kStandard,
        Map map = Map::kRandom,
        std::vector<FactionID> factions = {},
        Landmark landmark = Landmark::kNone,
        uint8_t landmark_clearing_index = 13,
        DeckType deck_type = DeckType::kStandard,
        uint8_t player_count = 4,
        bool double_vagabond = true
    );

    void setup_marquise_de_cat_starting_building(Building building, std::array<uint8_t, 12>& starting_buildings_indices);
    bool setup();

private:
    static constexpr uint8_t kOppositeCorner[12] = {
    8,  // 0 -> 8
    255,255, // 1,2 (not corners)
    11, // 3 -> 11
    255,255,255,255, // 4-7 (not corners)
    0,  // 8 -> 0
    255,255, // 9,10 (not corners)
    3   // 11 -> 3
    };

    static bool is_corner_clearing(uint8_t idx) {
        return idx == 0 || idx == 3 || idx == 8 || idx == 11;
    }

    std::mt19937 rng{std::random_device{}()};

    std::uniform_int_distribution<uint8_t> faction_dist{0, static_cast<uint8_t>(FactionID::kMaxFactionID) - 1};
    std::uniform_int_distribution<uint8_t> map_dist{0, static_cast<uint8_t>(Map::kMapCount) - 1};

    std::array<Clearing, kTotalClearings> clearings{};
    std::array<Forest, kTotalForests> forests{};
    SetupType setup_type;
    Map map;
    std::vector<FactionID> factions;
    Landmark landmark;
    uint8_t landmark_clearing_index;
    DeckType deck_type;
    uint8_t player_count;
    bool double_vagabond;
    std::vector<uint8_t> seating_order;
    std::vector<uint8_t> starting_clearing_indices;
};