#include <cstdint>

struct Clearing {
    /*
    24 bits: Connected Clearings {
        Connection (2 bits): {
            0 = No connection
            1 = Normal
            2 = By water
            3 = Blocked
        } x 12 for each clearing
    }
    24 bits: Building Slots (4 slots times 5 bits for building type, plus 1 bit for Elder Treetop Toggle) {
        Slot (6 bits): {
            Current Building (5 bits): {
                0 = Not a slot in this clearing
                1 = No building,
                2 = Ruin w/ Bag
                3 = Ruin w/ Sword
                4 = Ruin w/ Boots
                5 = Ruin w/ Hammer
                6 = Workshop,
                7 = Sawmill,
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
                18 = Figure / Waystation
                19 = Tablet / Jewelry Waystation
                20 = Jewelry / Figure Waystation
                21-31 = unused
            }  
            Is on Elder Treetop? (1 bit): {
                0 = No
                1 = Yes
            }
        } x 4 for each possible slot
    }
    Bits 23: Tokens {
        Bits 1-3: Wood (0-8 (9-15 unused))
        Bit 4: Keep (0-1)
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
        Bit 3: Jewelry Value 1 (0-1)
        Bit 4: Jewelry Value 2 (0-1)
        Bit 5: Jewelry Value 3 (0-2, 3 = unused)
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
        Bit 45: Lord of the Hundreds Warlord (0-1)
        Bits 46-49: Keepers in Iron Warriors (0-15)
    }
    Bits 50-51: Clearing Type {
        0 = Mouse
        1 = Fox
        2 = Rabbit
        3 = None
    }
    Bits 52-55: Connected Forests (0-8)
    Bit 56: Razed? (0-1)

    */
   int64_t data_continued;
};

class Game {
    public:
        Game() {

        }
};