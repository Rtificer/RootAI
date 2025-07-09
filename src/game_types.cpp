#include "../include/game_types.hpp"

// --- Clearing Implementation ---

template <typename T>
[[nodiscard]] T Clearing::read_bits(uint8_t shift, uint8_t width) const {
  uint64_t result = 0;
  uint8_t byte_index = shift / 8;
  uint8_t bit_offset = shift % 8;

  // Read bits across multiple bytes if necessary
  for (uint8_t i = 0; i < width; ++i) {
    uint8_t current_byte = byte_index + (bit_offset + i) / 8;
    uint8_t current_bit = (bit_offset + i) % 8;

    if (current_byte < data.size()) {
      uint8_t bit_value = (data[current_byte] >> current_bit) & 1;
      result |= (static_cast<uint64_t>(bit_value) << i);
    }
  }

  return static_cast<T>(result);
}

void Clearing::write_bits(uint8_t value, uint8_t shift, uint8_t width) {
  uint8_t byte_index = shift / 8;
  uint8_t bit_offset = shift % 8;

  // Write bits across multiple bytes if necessary
  for (uint8_t i = 0; i < width; ++i) {
    uint8_t current_byte = byte_index + (bit_offset + i) / 8;
    uint8_t current_bit = (bit_offset + i) % 8;

    if (current_byte < data.size()) {
      uint8_t bit_value = (value >> i) & 1;
      // Clear the bit first, then set it
      data[current_byte] &= ~(1 << current_bit);
      data[current_byte] |= (bit_value << current_bit);
    }
  }
}

[[nodiscard]] ClearingConnection Clearing::get_clearing_connection(
    uint8_t index) const {
  if (index >= kTotalClearings) {
    return ClearingConnection::kMaxConnectionIndex;
  }
  uint8_t shift = kConnectedClearingsShift + index * kClearingConnectionBits;
  return read_bits<ClearingConnection>(shift, kClearingConnectionBits);
}

bool Clearing::set_clearing_connection(uint8_t index,
                                       ClearingConnection value) {
  if (index < kTotalClearings) {
    uint64_t shift = kConnectedClearingsShift + index * kClearingConnectionBits;
    write_bits(static_cast<uint64_t>(value), shift, kClearingConnectionBits);
    return true;
  }
  return false;
}

[[nodiscard]] BuildingSlot Clearing::get_building_slot(uint8_t index) const {
  if (index >= kTotalBuildingSlots) {
    return {Building::kNotASlot, false};
  }
  Building building = read_bits<Building>(
      kBuildingSlotsShift + kBuildingSlotBits * index, kBuildingTypeBits);
  bool isTreetop = read_bits<bool>(
      kBuildingSlotsShift + (kBuildingSlotBits * index) + kBuildingTypeBits,
      kTreetopToggleBits);
  return {building, isTreetop};
}

bool Clearing::set_building_slot(uint8_t index, const BuildingSlot &slot) {
  if (index >= kTotalBuildingSlots) {
    return false;
  }
  uint8_t value =
      (static_cast<uint8_t>(slot.currentBuilding) & kBuildingTypeMask) |
      ((slot.isOnElderTreetop ? 1 : 0) << kBuildingTypeBits);
  uint8_t shift = kBuildingSlotsShift + index * kBuildingSlotBits;
  write_bits(value, shift, kBuildingSlotBits);
  return true;
}

[[nodiscard]] uint8_t Clearing::get_token_count(Token token) const {
  if (static_cast<uint8_t>(token) >=
      static_cast<uint8_t>(Token::kMaxTokenIndex)) {
    return 0;
  }
  const TokenInfo &info = kTokenFieldInfo[static_cast<uint8_t>(token)];
  return read_bits<uint8_t>(kTokensShift + info.offset, info.width);
}

bool Clearing::set_token_count(Token token, uint8_t count) {
  if (static_cast<uint8_t>(token) >=
      static_cast<uint8_t>(Token::kMaxTokenIndex))
    return false;
  const TokenInfo &info = kTokenFieldInfo[static_cast<uint8_t>(token)];
  write_bits(count, kTokensShift + info.offset, info.width);
  return true;
}

[[nodiscard]] uint8_t Clearing::get_pawn_count(FactionID factionID) const {
  if (static_cast<uint8_t>(factionID) >=
      static_cast<uint8_t>(FactionID::kMaxFactionID))
    return 0;
  const PawnInfo &info = kPawnFieldInfo[static_cast<uint8_t>(factionID)];
  uint8_t shift = kPawnDataShift + info.offset;
  return read_bits<uint8_t>(shift, info.width);
}

bool Clearing::set_pawn_count(FactionID factionID, uint8_t count) {
  if (static_cast<uint8_t>(factionID) >=
      static_cast<uint8_t>(FactionID::kMaxFactionID))
    return false;
  const PawnInfo &info = kPawnFieldInfo[static_cast<uint8_t>(factionID)];
  uint8_t shift = kPawnDataShift + info.offset;
  write_bits(count, shift, info.width);
  return true;
}

[[nodiscard]] bool Clearing::is_lord_of_the_hundreds_warlord_present() const {
  const PawnInfo &info =
      kPawnFieldInfo[static_cast<uint8_t>(FactionID::kLordOfTheHundreds) + 1];
  uint8_t shift = kPawnDataShift + info.offset;
  return read_bits<bool>(shift, info.width);
}

void Clearing::set_lord_of_the_hundreds_warlord_present(bool present) {
  const PawnInfo &info =
      kPawnFieldInfo[static_cast<uint8_t>(FactionID::kLordOfTheHundreds) + 1];
  uint8_t shift = kPawnDataShift + info.offset;
  write_bits(present ? 1 : 0, shift, info.width);
}

[[nodiscard]] ClearingType Clearing::get_clearing_type() const {
  return read_bits<ClearingType>(kClearingTypeShift, kClearingTypeBits);
}

void Clearing::set_clearing_type(ClearingType type) {
  write_bits(static_cast<uint8_t>(type), kClearingTypeShift, kClearingTypeBits);
}

[[nodiscard]] bool Clearing::get_connected_forest(uint8_t index) const {
  uint8_t shift = kConnectedForestsShift + index * kForestConnectionBits;
  return read_bits<bool>(shift, kForestConnectionBits);
}

void Clearing::set_connected_forest(uint8_t index, bool value) {
  uint8_t shift = kConnectedForestsShift + index * kForestConnectionBits;
  write_bits(value ? 1 : 0, shift, kForestConnectionBits);
}

[[nodiscard]] bool Clearing::is_razed() const {
  return read_bits<bool>(kRazedShift, kRazedBits);
}

void Clearing::set_razed(bool value) {
  write_bits(value ? 1 : 0, kRazedShift, kRazedBits);
}

[[nodiscard]] Landmark Clearing::get_landmark() const {
  return read_bits<Landmark>(kLandmarkShift, kLandmarkBits);
}

void Clearing::set_landmark(Landmark landmark) {
  write_bits(static_cast<uint8_t>(landmark), kLandmarkShift, kLandmarkBits);
}

[[nodiscard]] uint8_t Clearing::get_next_empty_building_slot_index(
    uint8_t starting_index) const {
  for (uint8_t i = starting_index; i < 4; ++i) {
    if (get_building_slot(i).currentBuilding == Building::kNoBuilding) {
      return i;
    }
  }
  return 255;  // No empty slot found
}

// --- Forest Implementation ---

template <typename T>
[[nodiscard]] T Forest::read_bits(uint8_t shift, uint8_t width) const {
  uint32_t mask = ((1UL << width) - 1) << shift;
  return static_cast<T>((data & mask) >> shift);
}

void Forest::write_bits(uint32_t value, uint8_t shift, uint8_t width) {
  uint32_t mask = ((1UL << width) - 1) << shift;
  data = (data & ~mask) | ((value << shift) & mask);
}

[[nodiscard]] std::array<Token,
                         (static_cast<uint8_t>(Token::kJewelryValue3) -
                          static_cast<uint8_t>(Token::kFigureValue1) + 1)>
Forest::get_relics() const {
  std::array<Token, static_cast<uint8_t>(Token::kJewelryValue3) -
                        static_cast<uint8_t>(Token::kFigureValue1) + 1>
      relics{};
  for (uint8_t i = static_cast<uint8_t>(Token::kFigureValue1);
       i <= static_cast<uint8_t>(Token::kJewelryValue3); ++i) {
    const TokenInfo &info = kTokenFieldInfo[i];
    uint8_t count = read_bits<uint8_t>(kRelicsShift + info.offset, info.width);
    relics.at(i) = static_cast<Token>(count);
  }
  return relics;
}

bool Forest::set_relic(Token token, uint8_t count) {
  if (static_cast<uint8_t>(token) <
          static_cast<uint8_t>(Token::kFigureValue1) ||
      static_cast<uint8_t>(token) >
          static_cast<uint8_t>(Token::kJewelryValue3)) {
    return false;
  }
  const TokenInfo &info = kTokenFieldInfo[static_cast<uint8_t>(token)];
  write_bits(count, kRelicsShift + info.offset, info.width);
  return true;
}

[[nodiscard]] bool Forest::get_vagabond(uint8_t index) const {
  if (index >= 2) {
    return false;
  }
  uint8_t shift = kVagabondsShift + index * kVagabondBits;
  return read_bits<bool>(shift, kVagabondBits);
}

bool Forest::set_vagabond(uint8_t index, bool value) {
  if (index >= 2) {
    return false;
  }
  uint8_t shift = kVagabondsShift + index * kVagabondBits;
  write_bits(value ? 1 : 0, shift, kVagabondBits);
  return true;
}

[[nodiscard]] bool Forest::get_connected_clearing(uint8_t index) const {
  if (index >= kTotalClearings) {
    return false;
  }
  uint8_t shift = kConnectedClearingsShift + index * kClearingConnectionBits;
  return read_bits<bool>(shift, kClearingConnectionBits);
}

bool Forest::set_connected_clearing(uint8_t index, bool value) {
  if (index >= kTotalClearings) {
    return false;
  }
  uint8_t shift = kConnectedClearingsShift + index * kClearingConnectionBits;
  write_bits(value ? 1 : 0, shift, kClearingConnectionBits);
  return true;
}

// kind suggestions to the compiler
template uint8_t Clearing::read_bits<uint8_t>(uint8_t, uint8_t) const;
template bool Clearing::read_bits<bool>(uint8_t, uint8_t) const;
template uint8_t Forest::read_bits<uint8_t>(uint8_t, uint8_t) const;
template bool Forest::read_bits<bool>(uint8_t, uint8_t) const;