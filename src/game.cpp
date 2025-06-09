#include <cstdint>

struct Clearing {
    /*
    Bits 1-24: Connected clearing indicies (4 bits for clearing index, times 6 possible connected clearings.  0-12 clearing indecies, 14 = blocked, 15 = not connected)
    Bit 25: Costal?
    Bits 26-40: Building Slots (3 slots times 4 bits for building type.) {
        0 = No building,
        1 = Ruin,
        2 = Workshop,
        3 = Sawmill,
        4 = Recruiter,
        5 = Roost,
        6 = Mouse Base,
        7 = Fox Base,
        8 = Rabbit Base,
        9 = Mouse Garden,
        10 = Fox Garden,
        11 = Rabbit Garden,
        12 = Citadel,
        13 = Market,
        14 = Stronghold,
        15 = Figure/ Waystation
        16 = Tablet/Jewlery Waystation
        17 = Jewlery/Figure Waystation
        18-31 = unused
    }
    Bits 41-63: Tokens {
        Bits 41-44: Wood (0-8 (9-15 unused))
        Bit 45: Sympathy (0-1)
        Bit 46: Mouse Trade Post (0-1)
        Bit 47: Fox Trade Post (0-1)
        Bit 48: Rabbit Trade Post (0-1)
        Bit 49: Bomb Plot (0-1)
        Bit 50-51: Snare Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bits 52-53: Extortion Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bits 54-55: Raid Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bit 56: Tunnel (0-1)
        Bit 57: Mob (0-1)
        Bit 58 Figure Value 1 (0-1)
        Bit 59 Figure Value 2 (0-1)
        Bits 60-61 Figure Value 3 (0-2, 3 = unused)
        Bit 62: Tablet Value 1 (0-1)
        Bit 63: Tablet Value 2 (0-1)
    }
    Bit 64: Unused
    */
    int64_t data;

    /*
    Bits 1-5: Tokens Continued {
        Bits 1-2: Tablet Value 3 (0-2, 3 = unused)
        Bit 3: Jewlery Value 1 (0-1)
        Bit 4: Jewlery Value 2 (0-1)
        Bit 5: Jewlery Value 3 (0-2, 3 = unused)
    }
    Bits 6-x Warrior Data {
        Bits 6-10: Marquise de Cat Warriors (0-25, 26-31 uneeded)
        Bits 11-15: Eyrie Dynasties Warriors (0-20, 21-31 uneeded)
        Bit 16: Vagabond 1 (0-1)
        Bit 17: Vagabond 2 (0-1)
        Bits 18-21: Woodland Alliance Warriors (0-10, 11-15 uneeded)
        Bits 22-25: Riverfolk Company Warriors (0-15)
        Bits 26-30: Lizard Cult Warriors (0-25, 26-31 uneeded)
        Bits 31-34: Corvid Conspiracy Warriors (0-15)
        Bits 35-39: Underground Duchy Warriors (0-20, 21-31 uneeded)
        Bits 40-44: Lord of the Hundreds Warriors (0-20, 21-31 uneeded)
        Bits 45-48: Keepers in Iron Warriors (0-15)
    }
    */
   int64_t data_continued;
};

class Game {
    public:
        Game() {

        }
};