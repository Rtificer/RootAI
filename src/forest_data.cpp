#include "../include/forest_data.hpp"

namespace game_data
{
namespace board_data
{
namespace forest_data
{
template<token_data::Token desiredRelic>
[[nodiscard]] inline std::expected<uint8_t, RelicError> Forest::get_relic_count() const
{
    static_assert(
        desiredRelic >= token_data::Token::kFigureValue1 && desiredRelic <= token_data::Token::kJewelryValue3,
        "Desired relic must be a token of relic type"
    );

    constexpr RelicDataInfo kRelicDataInfo = kRelicDataInfoField[
        static_cast<uint8_t>(desiredRelic) -
        static_cast<uint8_t>(token_data::Token::kFigureValue1)];

    const std::expected<uint8_t, RelicError> count = read_bits<uint8_t, kRelicDataInfo.offset + kRelicsOffset, kRelicDataInfo.width>();
    [[unlikely]] if (count.value() > kRelicDataInfo.maxCount)
        return std::unexpected(RelicError{RelicError::Code::kCountExceedsMaximum});

    return count;
}

template<token_data::Token desiredRelic, uint8_t newCount>
inline void Forest::set_relic_count()
{
    static_assert(
        desiredRelic >= token_data::Token::kFigureValue1 && desiredRelic <= token_data::Token::kJewelryValue3,
        "Desired relic must be a token of relic type"
    );

    constexpr RelicDataInfo kRelicDataInfo = kRelicDataInfoField[
        static_cast<uint8_t>(desiredRelic) -
        static_cast<uint8_t>(token_data::Token::kFigureValue1)];
    
    static_assert(newCount <= kRelicDataInfo.maxCount, "Cannot set relic count above maximum relic count for that relic");

    write_bits<uint8_t, kRelicDataInfo.offset + kRelicsOffset, kRelicDataInfo.width>(newCount);
}

template<token_data::Token desiredRelic>
[[nodiscard]] inline std::expected<void, RelicError> Forest::set_relic_count(uint8_t newCount)
{
    static_assert(
        (desiredRelic >= token_data::Token::kFigureValue1) && 
        (desiredRelic <= token_data::Token::kJewelryValue3),
        "Desired relic must be a token of relic type"
    );

    constexpr RelicDataInfo kRelicDataInfo = kRelicDataInfoField[
        static_cast<uint8_t>(desiredRelic) - 
        static_cast<uint8_t>(token_data::Token::kFigureValue1)];
    
    [[unlikely]] if (newCount > kRelicDataInfo.maxCount)
        return std::unexpected(RelicError{RelicError::Code::kCountExceedsMaximum});

    write_bits<uint8_t, kRelicDataInfo.offset + kRelicsOffset, kRelicDataInfo.width>(newCount);
    return {};
}


template <token_data::RelicType relicType>
[[nodiscard]] uint8_t Forest::get_relic_type_count() const
{
    if constexpr (relicType == token_data::RelicType::kFigure)
    {
        constexpr uint8_t kFigureValue1Offset = 
            static_cast<uint8_t>(token_data::Token::kFigureValue1) - 
            static_cast<uint8_t>(token_data::Token::kFigureValue1);

        constexpr uint8_t kFigureValue2Offset =
            static_cast<uint8_t>(token_data::Token::kFigureValue2) - 
            static_cast<uint8_t>(token_data::Token::kFigureValue1);

        constexpr uint8_t kFigureValue3Offset =
            static_cast<uint8_t>(token_data::Token::kFigureValue3) - 
            static_cast<uint8_t>(token_data::Token::kFigureValue1);

        constexpr uint8_t kTotalFigureBits = 
            kRelicDataInfoField[kFigureValue1Offset].width +
            kRelicDataInfoField[kFigureValue2Offset].width +
            kRelicDataInfoField[kFigureValue3Offset].width;
        static_assert(kTotalFigureBits > 0 && kTotalFigureBits <= 8, "Total Figure bits must not exceed one byte");

        const uint8_t combined = read_bits<uint8_t, kFigureValue1Offset + kRelicsOffset, kTotalFigureBits>();

        constexpr uint8_t kFigureValue1Width = kRelicDataInfoField[kFigureValue1Offset].width;
        constexpr uint8_t kFigureValue2Width = kRelicDataInfoField[kFigureValue2Offset].width;
        constexpr uint8_t kFigureValue3Width = kRelicDataInfoField[kFigureValue3Offset].width;

        return
            (combined & ((1U << kFigureValue1Width) - 1)) + // Figure Value 1 Count
            ((combined >> kFigureValue1Width) & ((1U << kFigureValue2Width) - 1)) + // Figure Value 2 Count
            ((combined >> (kFigureValue1Width + kFigureValue2Width)) & ((1U << kFigureValue3Width) - 1)); // Figure Value 3
    } else if constexpr (relicType == token_data::RelicType::kTablet)
    {
        constexpr uint8_t kTabletValue1Offset = 
            static_cast<uint8_t>(token_data::Token::kTabletValue1) - 
            static_cast<uint8_t>(token_data::Token::kFigureValue1);

        constexpr uint8_t kTabletValue2Offset =
            static_cast<uint8_t>(token_data::Token::kTabletValue2) - 
            static_cast<uint8_t>(token_data::Token::kFigureValue1);

        constexpr uint8_t kTabletValue3Offset =
            static_cast<uint8_t>(token_data::Token::kTabletValue3) - 
            static_cast<uint8_t>(token_data::Token::kFigureValue1);

        constexpr uint8_t kTotalTabletBits = 
            kRelicDataInfoField[kTabletValue1Offset].width +
            kRelicDataInfoField[kTabletValue2Offset].width +
            kRelicDataInfoField[kTabletValue3Offset].width;
        static_assert(kTotalTabletBits > 0 && kTotalTabletBits <= 8, "Total Tablet bits must not exceed one byte");

        const uint8_t combined = read_bits<uint8_t, kTabletValue1Offset + kRelicsOffset, kTotalTabletBits>();

        constexpr uint8_t kTabletValue1Width = kRelicDataInfoField[kTabletValue1Offset].width;
        constexpr uint8_t kTabletValue2Width = kRelicDataInfoField[kTabletValue2Offset].width;
        constexpr uint8_t kTabletValue3Width = kRelicDataInfoField[kTabletValue3Offset].width;

        return
            (combined & ((1U << kTabletValue1Width) - 1)) + // Tablet Value 1 Count
            ((combined >> kTabletValue1Width) & ((1U << kTabletValue2Width) - 1)) + // Tablet Value 2 Count
            ((combined >> (kTabletValue1Width + kTabletValue2Width)) & ((1U << kTabletValue3Width) - 1)); // Tablet Value 3
    } else {
        constexpr uint8_t kJewelryValue1Offset = 
            static_cast<uint8_t>(token_data::Token::kJewelryValue1) - 
            static_cast<uint8_t>(token_data::Token::kFigureValue1);

        constexpr uint8_t kJewelryValue2Offset =
            static_cast<uint8_t>(token_data::Token::kJewelryValue2) - 
            static_cast<uint8_t>(token_data::Token::kFigureValue1);

        constexpr uint8_t kJewelryValue3Offset =
            static_cast<uint8_t>(token_data::Token::kJewelryValue3) - 
            static_cast<uint8_t>(token_data::Token::kFigureValue1);

        constexpr uint8_t kTotalJewelryBits = 
            kRelicDataInfoField[kJewelryValue1Offset].width +
            kRelicDataInfoField[kJewelryValue2Offset].width +
            kRelicDataInfoField[kJewelryValue3Offset].width;
        static_assert(kTotalJewelryBits > 0 && kTotalJewelryBits <= 8, "Total Jewelry bits must not exceed one byte");

        const uint8_t combined = read_bits<uint8_t, kJewelryValue1Offset + kRelicsOffset, kTotalJewelryBits>();

        constexpr uint8_t kJewelryValue1Width = kRelicDataInfoField[kJewelryValue1Offset].width;
        constexpr uint8_t kJewelryValue2Width = kRelicDataInfoField[kJewelryValue2Offset].width;
        constexpr uint8_t kJewelryValue3Width = kRelicDataInfoField[kJewelryValue3Offset].width;

        return 
            static_cast<uint8_t>(combined & ((1U << kJewelryValue1Width) - 1)) + // Jewelry Value 1 Count
            static_cast<uint8_t>((combined >> kJewelryValue1Width) & ((1U << kJewelryValue2Width) - 1)) + // Jewelry Value 2 Count
            static_cast<uint8_t>((combined >> (kJewelryValue1Width + kJewelryValue2Width)) & ((1U << kJewelryValue3Width) - 1)); // Jewelry Value 3
    }
}

template <uint8_t whichVagabond>
[[nodiscard]] bool Forest::is_vagabond_present() const
{
    static_assert(kVagabondBits == 1, "kVagabondBits must be equal to one");
    static_assert(whichVagabond == 1 || whichVagabond == 2, "whichVagabond must be either 1 or 2");
    
    if constexpr (whichVagabond == 1)
        return read_bits<bool, kVagabondsOffset, kVagabondBits>();

    return read_bits<bool, kVagabondsOffset + kVagabondBits, kVagabondBits>();
}

template <uint8_t whichVagabond>
void Forest::set_is_vagabond_present(bool value)
{
    static_assert(kVagabondBits == 1, "kVagabondBits must be equal to one");
    static_assert(whichVagabond == 1 || whichVagabond == 2, "whichVagabond must be either 1 or 2");

    if constexpr (whichVagabond == 1)
        return write_bits<bool, kVagabondsOffset, kVagabondBits>(value);

    return write_bits<bool, kVagabondsOffset + kVagabondBits, kVagabondBits>(value);
}
} // forest_data
} // board_data
} // game_data