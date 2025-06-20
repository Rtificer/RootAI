#include "game.hpp"

Game::Game(
    SetupType setup_type,
    Map map,
    std::vector<FactionID> factions,
    Landmark landmark,
    uint8_t landmark_clearing_index,
    DeckType deck_type,
    uint8_t player_count,
    bool double_vagabond)
    : setup_type(setup_type),
      map(map),
      factions(factions),
      landmark(landmark),
      landmark_clearing_index(landmark_clearing_index),
      deck_type(deck_type),
      player_count(player_count),
      double_vagabond(double_vagabond)
{
}

void Game::setup_marquise_de_cat_starting_building(Building building, std::array<uint8_t, 12> &starting_buildings_indices)
{
    while (true)
    {
        switch (building)
        {
        case Building::Sawmill:
            std::cout << "Choose Marquise de Cat Sawmill Clearing" << std::endl;
            break;
        case Building::Workshop:
            std::cout << "Choose Marquise de Cat Workshop Clearing" << std::endl;
            break;
        case Building::Recruiter:
            std::cout << "Choose Marquise de Cat Recruiter Clearing" << std::endl;
            break;
        }
        uint8_t building_clearing_index;
        std::cin >> building_clearing_index;
        if (building_clearing_index < 12 && starting_buildings_indices[building_clearing_index] != 255)
        {
            clearings[building_clearing_index].set_building_slot(
                starting_buildings_indices[building_clearing_index],
                {building, clearings[building_clearing_index].get_building_slot(starting_buildings_indices[building_clearing_index]).isOnElderTreetop});

            starting_buildings_indices[building_clearing_index] = clearings[building_clearing_index].get_next_empty_building_slot_index(starting_buildings_indices[building_clearing_index]);
            break;
        }
        std::cout << "Invalid clearing index. Please choose a valid building clearing." << std::endl;
    }
}

bool Game::setup()
{
    if (map == Map::Random)
        map = static_cast<Map>(map_dist(rng));

    switch (setup_type)
    {
    case SetupType::Standard:
        if (factions.size() > player_count - 1)
            return false;

        seating_order.resize(player_count);
        starting_clearing_indices.resize(player_count, 0);

        bool has_vagabond1 = std::find(factions.begin(), factions.end(), FactionID::Vagabond1) != factions.end();
        bool has_vagabond2 = std::find(factions.begin(), factions.end(), FactionID::Vagabond2) != factions.end();

        while (factions.size() < player_count - 1)
        {
            FactionID potential_faction = static_cast<FactionID>(faction_dist(rng));

            if (potential_faction == FactionID::Vagabond1)
            {
                if (!has_vagabond1)
                {
                    factions.push_back(FactionID::Vagabond1);
                    has_vagabond1 = true;
                }
            }
            else if (potential_faction == FactionID::Vagabond2)
            {
                if (double_vagabond && has_vagabond1 && !has_vagabond2)
                {
                    factions.push_back(FactionID::Vagabond2);
                    has_vagabond2 = true;
                }
            }
            else
            {
                if (std::find(factions.begin(), factions.end(), potential_faction) == factions.end())
                {
                    factions.push_back(potential_faction);
                }
            }
        }

        std::iota(seating_order.begin(), seating_order.end(), 0);
        std::shuffle(seating_order.begin(), seating_order.end(), rng);

        auto marquise_pointer = std::find(factions.begin(), factions.end(), static_cast<FactionID>
        (FactionID::MarquiseDeCat));
        uint8_t marquise_index = 255;
        if (marquise_pointer != factions.end())
        {
            marquise_index = std::distance(factions.begin(), marquise_pointer);
            while (true)
            {
                uint8_t clearing_index;
                std::cout << "Choose Marquise de Cat Keep Clearing" << std::endl;
                std::cin >> clearing_index;
                if (is_corner_clearing(clearing_index))
                {
                    starting_clearing_indices.at(marquise_index) = clearing_index;
                    break;
                }
                std::cout << "Invalid clearing index. Please choose a corner clearing (0, 3, 8, or 11)." << std::endl;
            }

            for (uint8_t j = 0; j < 12; ++j)
            {
                if (j != OPPOSITE_CORNER[starting_clearing_indices.at(marquise_index)])
                {
                    clearings[j].set_pawn_count(FactionID::MarquiseDeCat, 1);
                }
            }

            // index = clearing, value = valid building index, 255 = invalid
            std::array<uint8_t, 12> starting_buildings_indices;
            for (uint8_t j = 0; j < 12; ++j)
            {
                if (clearings[starting_clearing_indices.at(marquise_index)].get_clearing_connection(j) == ClearingConnection::Normal)
                {
                    starting_buildings_indices[j] = clearings[j].get_next_empty_building_slot_index(0);
                }
            }

            setup_marquise_de_cat_starting_building(Building::Sawmill, starting_buildings_indices);
            setup_marquise_de_cat_starting_building(Building::Workshop, starting_buildings_indices);
            setup_marquise_de_cat_starting_building(Building::Recruiter, starting_buildings_indices);
        }

        auto eyrie_pointer = std::find(factions.begin(), factions.end(), static_cast<FactionID>(FactionID::EyrieDynasties));
        uint8_t eyrie_index = 255;
        if (eyrie_pointer != factions.end())
        {
            eyrie_index = std::distance(factions.begin(), eyrie_pointer);
            if (marquise_pointer == factions.end())
            {
                while (true)
                {
                    uint8_t clearing_index;
                    std::cout << "Choose Eyrie Dynasties Starting Clearing" << std::endl;
                    std::cin >> clearing_index;
                    if (is_corner_clearing(clearing_index))
                    {
                        starting_clearing_indices.at(eyrie_index) = clearing_index;
                        break;
                    }
                    std::cout << "Invalid clearing index. Please choose a corner clearing (0, 3, 8, or 11)." << std::endl;
                }
            }
            else
            {
                starting_clearing_indices.at(eyrie_index) = OPPOSITE_CORNER[starting_clearing_indices.at(marquise_index)];
            }

            clearings[starting_clearing_indices.at(eyrie_index)].set_pawn_count(FactionID::EyrieDynasties, 6);

            //TODO: Choose Eyrie Leader
            //Select Visors for that leader
        }

        break;
    case SetupType::Advanced:
        // Initialize advanced setup
        break;
    default:
        return false; // Invalid setup type
    }
}