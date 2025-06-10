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

    28 Bits: Tokens {
        Bits 1-3: Wood (0-8 (9-15 unused))
        Bit 4: Keep (0-1)
        Bit 5: Sympathy (0-1)
        Bit 6: Mouse Trade Post (0-1)
        Bit 7: Fox Trade Post (0-1)
        Bit 8: Rabbit Trade Post (0-1)
        Bit 9: Bomb Plot (0-1)
        Bit 10-11: Snare Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bits 12-13: Extortion Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bits 14-15: Raid Plot (0 = none, 1 = Face Down, 2 = Face Up, 3 = unused)
        Bit 16: Tunnel (0-1)
        Bit 17: Mob (0-1)
        Bit 18 Figure Value 1 (0-1)
        Bit 19 Figure Value 2 (0-1)
        Bits 20-21 Figure Value 3 (0-2, 3 = unused)
        Bit 22: Tablet Value 1 (0-1)
        Bit 23: Tablet Value 2 (0-1)
        Bits 24-25: Tablet Value 3 (0-2, 3 = unused)
        Bit 26: Jewelry Value 1 (0-1)
        Bit 27: Jewelry Value 2 (0-1)
        Bit 28: Jewelry Value 3 (0-2, 3 = unused)
    }

    44 Bits Warrior Data {
        Bits 1-5: Marquise de Cat Warriors (0-25, 26-31 uneeded)
        Bits 6-10: Eyrie Dynasties Warriors (0-20, 21-31 uneeded)
        Bit 11: Vagabond 1 (0-1)
        Bit 12: Vagabond 2 (0-1)
        Bits 13-16: Woodland Alliance Warriors (0-10, 11-15 uneeded)
        Bits 17-20: Riverfolk Company Warriors (0-15)
        Bits 21-25: Lizard Cult Warriors (0-25, 26-31 uneeded)
        Bits 26-29: Corvid Conspiracy Warriors (0-15)
        Bits 30-34: Underground Duchy Warriors (0-20, 21-31 uneeded)
        Bits 35-39: Lord of the Hundreds Warriors (0-20, 21-31 uneeded)
        Bit 40: Lord of the Hundreds Warlord (0-1)
        Bits 41-44: Keepers in Iron Warriors (0-15)
    }

    2 Bits: Clearing Type {
        0 = Mouse
        1 = Fox
        2 = Rabbit
        3 = None
    }

    24 Bits: Connected Forests {
        4 Bits: Clearing Index (0-12, 13-14 unused)
    } x 6 for each possible connection

    1 Bit: Razed? (0-1)
    */
};

class Game {
    public:
        Game() {

        }
};