#include "../include/game_types.hpp"

// --- Clearing Implementation ---

Clearing::Clearing() : data1(0), data2(0), data3(0) {}

template <typename T>
T Clearing::read_bits(uint8_t shift, uint8_t width) const
{
    if (shift + width <= 64)
    {
        uint64_t mask = ((1ULL << width) - 1) << shift;
        return static_cast<T>((data1 & mask) >> shift);
    }
    else if (shift < 64 && shift + width <= 128)
    {
        uint8_t bits_in_data1 = 64 - shift;
        uint8_t bits_in_data2 = width - bits_in_data1;
        uint64_t mask1 = ((1ULL << bits_in_data1) - 1) << shift;
        uint64_t mask2 = (1ULL << bits_in_data2) - 1;
        uint64_t part1 = (data1 & mask1) >> shift;
        uint64_t part2 = (data2 & mask2);
        return static_cast<T>(part1 | (part2 << bits_in_data1));
    }
    else if (shift + width <= 128)
    {
        uint8_t shift2 = shift - 64;
        uint64_t mask = ((1ULL << width) - 1) << shift2;
        return static_cast<T>((data2 & mask) >> shift2);
    }
    else if (shift < 128 && shift + width <= 192)
    {
        uint8_t bits_in_data2 = 128 - shift;
        uint8_t bits_in_data3 = width - bits_in_data2;
        uint64_t mask2 = ((1ULL << bits_in_data2) - 1) << (shift - 64);
        uint32_t mask3 = (1U << bits_in_data3) - 1;
        uint64_t part2 = (data2 & mask2) >> (shift - 64);
        uint32_t part3 = (data3 & mask3);
        return static_cast<T>(part2 | (static_cast<uint64_t>(part3) << bits_in_data2));
    }
    else
    {
        uint8_t shift3 = shift - 128;
        uint32_t mask = ((1U << width) - 1) << shift3;
        return static_cast<T>((data3 & mask) >> shift3);
    }
}

void Clearing::write_bits(uint64_t value, uint8_t shift, uint8_t width)
{
    if (shift + width <= 64)
    {
        uint64_t mask = ((1ULL << width) - 1) << shift;
        data1 = (data1 & ~mask) | ((value << shift) & mask);
    }
    else if (shift < 64 && shift + width <= 128)
    {
        uint8_t bits_in_data1 = 64 - shift;
        uint8_t bits_in_data2 = width - bits_in_data1;
        uint64_t mask1 = ((1ULL << bits_in_data1) - 1) << shift;
        uint64_t mask2 = (1ULL << bits_in_data2) - 1;
        data1 = (data1 & ~mask1) | ((value << shift) & mask1);
        data2 = (data2 & ~mask2) | ((value >> bits_in_data1) & mask2);
    }
    else if (shift + width <= 128)
    {
        uint8_t shift2 = shift - 64;
        uint64_t mask = ((1ULL << width) - 1) << shift2;
        data2 = (data2 & ~mask) | ((value << shift2) & mask);
    }
    else if (shift < 128 && shift + width <= 192)
    {
        uint8_t bits_in_data2 = 128 - shift;
        uint8_t bits_in_data3 = width - bits_in_data2;
        uint64_t mask2 = ((1ULL << bits_in_data2) - 1) << (shift - 64);
        uint32_t mask3 = (1U << bits_in_data3) - 1;
        data2 = (data2 & ~mask2) | ((value << (shift - 64)) & mask2);
        data3 = (data3 & ~mask3) | ((static_cast<uint32_t>(value >> bits_in_data2)) & mask3);
    }
    else
    {
        uint8_t shift3 = shift - 128;
        uint32_t mask = ((1U << width) - 1) << shift3;
        data3 = (data3 & ~mask) | ((static_cast<uint32_t>(value) << shift3) & mask);
    }
}

ClearingConnection Clearing::get_clearing_connection(uint8_t index) const
{
    if (index >= TOTAL_CLEARINGS)
    {
        return ClearingConnection::MaxConnectionIndex;
    }
    uint8_t shift = CONNECTED_CLEARINGS_SHIFT + index * CLEARING_CONNECTION_BITS;
    return read_bits<ClearingConnection>(shift, CLEARING_CONNECTION_BITS);
}

bool Clearing::set_clearing_connection(uint8_t index, ClearingConnection value)
{
    if (index < TOTAL_CLEARINGS)
    {
        uint64_t shift = CONNECTED_CLEARINGS_SHIFT + index * CLEARING_CONNECTION_BITS;
        write_bits(static_cast<uint64_t>(value), shift, CLEARING_CONNECTION_BITS);
        return true;
    }
    return false;
}

BuildingSlot Clearing::get_building_slot(uint8_t index) const
{
    if (index >= TOTAL_BUILDING_SLOTS)
    {
        return {Building::NotASlot, false};
    }
    Building building = read_bits<Building>(BUILDING_SLOTS_SHIFT + BUILDING_SLOT_BITS * index, BUILDING_TYPE_BITS);
    bool isTreetop = read_bits<bool>(BUILDING_SLOTS_SHIFT + (BUILDING_SLOT_BITS * index) + BUILDING_TYPE_BITS, TREETOP_TOGGLE_BITS);
    return {building, isTreetop};
}

bool Clearing::set_building_slot(uint8_t index, const BuildingSlot &slot)
{
    if (index >= TOTAL_BUILDING_SLOTS)
    {
        return false;
    }
    uint8_t value = (static_cast<uint8_t>(slot.currentBuilding) & BUILDING_TYPE_MASK) | ((slot.isOnElderTreetop ? 1 : 0) << BUILDING_TYPE_BITS);
    uint8_t shift = BUILDING_SLOTS_SHIFT + index * BUILDING_SLOT_BITS;
    write_bits(value, shift, BUILDING_SLOT_BITS);
    return true;
}

uint8_t Clearing::get_token_count(Token token) const
{
    if (static_cast<uint8_t>(token) >= static_cast<uint8_t>(Token::MaxTokenIndex))
    {
        return 0;
    }
    const TokenInfo &info = TOKEN_FIELD_INFO[static_cast<uint8_t>(token)];
    return read_bits<uint8_t>(TOKENS_SHIFT + info.offset, info.width);
}

bool Clearing::set_token_count(Token token, uint8_t count)
{
    if (static_cast<uint8_t>(token) >= static_cast<uint8_t>(Token::MaxTokenIndex))
        return false;
    const TokenInfo &info = TOKEN_FIELD_INFO[static_cast<uint8_t>(token)];
    write_bits(count, TOKENS_SHIFT + info.offset, info.width);
    return true;
}

uint8_t Clearing::get_pawn_count(FactionID factionID) const
{
    if (static_cast<uint8_t>(factionID) >= static_cast<uint8_t>(FactionID::MaxFactionID))
        return 0;
    const PawnInfo &info = PAWN_FIELD_INFO[static_cast<uint8_t>(factionID)];
    uint8_t shift = PAWN_DATA_SHIFT + info.offset;
    return read_bits<uint8_t>(shift, info.width);
}

bool Clearing::set_pawn_count(FactionID factionID, uint8_t count)
{
    if (static_cast<uint8_t>(factionID) >= static_cast<uint8_t>(FactionID::MaxFactionID))
        return false;
    const PawnInfo &info = PAWN_FIELD_INFO[static_cast<uint8_t>(factionID)];
    uint8_t shift = PAWN_DATA_SHIFT + info.offset;
    write_bits(count, shift, info.width);
    return true;
}

bool Clearing::is_lord_of_the_hundreds_warlord_present() const
{
    const PawnInfo &info = PAWN_FIELD_INFO[static_cast<uint8_t>(FactionID::LordOfTheHundreds) + 1];
    uint8_t shift = PAWN_DATA_SHIFT + info.offset;
    return read_bits<bool>(shift, info.width);
}

void Clearing::set_lord_of_the_hundreds_warlord_present(bool present)
{
    const PawnInfo &info = PAWN_FIELD_INFO[static_cast<uint8_t>(FactionID::LordOfTheHundreds) + 1];
    uint8_t shift = PAWN_DATA_SHIFT + info.offset;
    write_bits(present ? 1 : 0, shift, info.width);
}

ClearingType Clearing::get_clearing_type() const
{
    return read_bits<ClearingType>(CLEARING_TYPE_SHIFT, CLEARING_TYPE_BITS);
}

void Clearing::set_clearing_type(ClearingType type)
{
    write_bits(static_cast<uint8_t>(type), CLEARING_TYPE_SHIFT, CLEARING_TYPE_BITS);
}

bool Clearing::get_connected_forest(uint8_t index) const
{
    uint8_t shift = CONNECTED_FORESTS_SHIFT + index * FOREST_CONNECTION_BITS;
    return read_bits<bool>(shift, FOREST_CONNECTION_BITS);
}

void Clearing::set_connected_forest(uint8_t index, bool value)
{
    uint8_t shift = CONNECTED_FORESTS_SHIFT + index * FOREST_CONNECTION_BITS;
    write_bits(value ? 1 : 0, shift, FOREST_CONNECTION_BITS);
}

bool Clearing::is_razed() const
{
    return read_bits<bool>(RAZED_SHIFT, RAZED_BITS);
}

void Clearing::set_razed(bool value)
{
    write_bits(value ? 1 : 0, RAZED_SHIFT, RAZED_BITS);
}

Landmark Clearing::get_landmark() const
{
    return read_bits<Landmark>(LANDMARK_SHIFT, LANDMARK_BITS);
}

void Clearing::set_landmark(Landmark landmark)
{
    write_bits(static_cast<uint8_t>(landmark), LANDMARK_SHIFT, LANDMARK_BITS);
}

uint8_t Clearing::get_next_empty_building_slot_index(uint8_t starting_index) const
{
    for (uint8_t i = starting_index; i < 4; ++i)
    {
        if (get_building_slot(i).currentBuilding == Building::NoBuilding)
        {
            return i;
        }
    }
    return 255; // No empty slot found
}

// --- Forest Implementation ---

Forest::Forest() : data(0) {}

template <typename T>
T Forest::read_bits(uint8_t shift, uint8_t width) const
{
    uint32_t mask = ((1UL << width) - 1) << shift;
    return static_cast<T>((data & mask) >> shift);
}

void Forest::write_bits(uint32_t value, uint8_t shift, uint8_t width)
{
    uint32_t mask = ((1UL << width) - 1) << shift;
    data = (data & ~mask) | ((value << shift) & mask);
}

std::array<Token, (static_cast<uint8_t>(Token::JewelryValue3) - static_cast<uint8_t>(Token::FigureValue1) + 1)> Forest::get_relics() const
{
    std::array<Token, static_cast<uint8_t>(Token::JewelryValue3) - static_cast<uint8_t>(Token::FigureValue1) + 1> relics{};
    for (uint8_t i = static_cast<uint8_t>(Token::FigureValue1); i <= static_cast<uint8_t>(Token::JewelryValue3); ++i)
    {
        const TokenInfo &info = TOKEN_FIELD_INFO[i];
        uint8_t count = read_bits<uint8_t>(RELICS_SHIFT + info.offset, info.width);
        relics.at(i) = static_cast<Token>(count);
    }
    return relics;
}

bool Forest::set_relic(Token token, uint8_t count)
{
    if (static_cast<uint8_t>(token) < static_cast<uint8_t>(Token::FigureValue1) || static_cast<uint8_t>(token) > static_cast<uint8_t>(Token::JewelryValue3)) {
        return false;
    }
    const TokenInfo &info = TOKEN_FIELD_INFO[static_cast<uint8_t>(token)];
    write_bits(count, RELICS_SHIFT + info.offset, info.width);
    return true;
}

bool Forest::get_vagabond(uint8_t index) const
{
    if (index >= 2) {
        return false;
    }
    uint8_t shift = VAGABONDS_SHIFT + index * VAGABOND_BITS;
    return read_bits<bool>(shift, VAGABOND_BITS);
}

bool Forest::set_vagabond(uint8_t index, bool value)
{
    if (index >= 2) {
        return false;
    }
    uint8_t shift = VAGABONDS_SHIFT + index * VAGABOND_BITS;
    write_bits(value ? 1 : 0, shift, VAGABOND_BITS);
    return true;
}

bool Forest::get_connected_clearing(uint8_t index) const
{
    if (index >= TOTAL_CLEARINGS) {
        return false;
    }
    uint8_t shift = CONNECTED_CLEARINGS_SHIFT + index * CLEARING_CONNECTION_BITS;
    return read_bits<bool>(shift, CLEARING_CONNECTION_BITS);
}

bool Forest::set_connected_clearing(uint8_t index, bool value)
{
    if (index >= TOTAL_CLEARINGS) {
        return false;
    }
    uint8_t shift = CONNECTED_CLEARINGS_SHIFT + index * CLEARING_CONNECTION_BITS;
    write_bits(value ? 1 : 0, shift, CLEARING_CONNECTION_BITS);
    return true;
}

// kind suggestions to the compiler
template uint8_t Clearing::read_bits<uint8_t>(uint8_t, uint8_t) const;
template bool Clearing::read_bits<bool>(uint8_t, uint8_t) const;
template uint8_t Forest::read_bits<uint8_t>(uint8_t, uint8_t) const;
template bool Forest::read_bits<bool>(uint8_t, uint8_t) const;