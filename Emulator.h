//
// Created by lucas on 7/14/15.
//

#ifndef EMULATOR_H
#define EMULATOR_H

#include <stddef.h>
#include <stdlib.h>

#include "GameScreen.h"
#include <SDL2/SDL_keycode.h>

class chip_8_emu {

    uint8_t charSprites[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // Define them as uint instead of char, so no casting difficulties
    uint8_t RAM[4096];

    uint8_t Registers[16];
    uint8_t timer, soundTimer;
    uint16_t Stack[16];
    uint16_t I, VF, PC, SP;
    uint8_t keyboard;


    uint8_t **screen;
    GameScreen *GS;


    void panic();

public:
    bool STOP;

    chip_8_emu(GameScreen *GS);

    void tick();

    int loadRom(const char *file);

    void setKeyPress(SDL_Keycode key);

    void releaseKey(SDL_Keycode key);

};


#endif //EMULATOR

// RAM LAYOUT
// +---------------+= 0xFFF (4095) End of Chip-8 RAM
// |               |
// |               |
// |               |
// |               |
// |               |
// | 0x200 to 0xFFF|
// |     Chip-8    |
// | Program / Data|
// |     Space     |
// |               |
// |               |
// |               |
// +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
// |               |
// |               |
// |               |
// +---------------+= 0x200 (512) Start of most Chip-8 programs
// | 0x000 to 0x1FF|
// | Reserved for  |
// |  interpreter  |
// +---------------+= 0x000 (0) Start of Chip-8 RAM