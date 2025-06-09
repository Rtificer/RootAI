#include <cstdint>

struct Clearing {
    /*
    Bits 1-24: Connected clearing indicies (4 bits for clearing index, times 6 possible connected clearings.  0-12 clearing indecies, 14 = blocked, 15 = not connected)
    Bit 25: Costal?
    Bits 26-40: Building Slots (3 slots times 4 bits for building type.) {
        0 = No building,
        1 = Ruin w/ Bag
        2 = Ruin w/ Sword
        3 = Ruin w/ Boots
        4 = Ruin w/ Hammer
        5 = Workshop,
        6 = Sawmill,
        7 = Recruiter,
        8 = Roost,
        9 = Mouse Base,
        10 = Fox Base,
        11 = Rabbit Base,
        12 = Mouse Garden,
        13 = Fox Garden,
        14 = Rabbit Garden,
        15 = Citadel,
        16 = Market,
        17 = Stronghold,
        18 = Figure/ Waystation
        19 = Tablet/Jewlery Waystation
        20 = Jewlery/Figure Waystation
        21-31 = unused
    }
    Bits 41-63: Tokens {
        Bits 41-44: Wood (0-8 (9-15 unused))
        Bit 45: Keep (0-1)
        Bit 46: Sympathy (0-1)
        Bit 47: Mouse Trade Post (0-1)
        Bit 48: Fox Trade Post (0-1)
        Bit 49: Rabbit Trade Post (0-1)
        Bit 50: Bomb Plot (0-1)
        Bit 51-52: Snare Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bits 53-54: Extortion Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bits 55-56: Raid Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bit 57: Tunnel (0-1)
        Bit 58: Mob (0-1)
        Bit 59 Figure Value 1 (0-1)
        Bit 60 Figure Value 2 (0-1)
        Bits 61-62 Figure Value 3 (0-2, 3 = unused)
        Bit 63: Tablet Value 1 (0-1)
        Bit 64: Tablet Value 2 (0-1)
    }
    */
    int64_t data;

    /*
    Bits 1-5: Tokens Continued {
        Bits 1-2: Tablet Value 3 (0-2, 3 = unused)
        Bit 3: Jewlery Value 1 (0-1)
        Bit 4: Jewlery Value 2 (0-1)
        Bit 5: Jewlery Value 3 (0-2, 3 = unused)
    }
    Bits 6-48 Warrior Data {
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