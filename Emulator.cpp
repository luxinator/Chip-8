
#include <iostream>
#include "Emulator.h"
#include <time.h>



chip_8_emu::chip_8_emu(GameScreen * GS){

	std::cout << "Setting up Chip8 Emulator" << std::endl;

	memset(RAM, 0, sizeof(uint8_t) * 4096);
	memset(Stack, 0, sizeof(uint16_t) * 16);
	memset(Registers, 0, sizeof(uint8_t) * 16);

	//Special Registers
	I = 0x0;
	VF = 0x0;
	PC = 0x0;
	SP = 0;
    keyboard = 0;

	//Fill in the Standard Character Sprites
	for(size_t i = 0; i < 80; i++)
		RAM[i] = charSprites[i];

	this->GS = GS;
	this->screen = GS->getStates();

	PC = 0x200;
    time_t seconds;
    time(&seconds);

	srandom((uint32_t) seconds);
    this->STOP = false;
	
}

void chip_8_emu::tick() {

    if (this->timer)
        this->timer--;
    if (this->soundTimer)
        this->soundTimer--;

    uint16_t opcode = RAM[PC] << 8 | RAM[PC + 1];

    //
    //A byte in chip8 is 16-bit we read in words of 8-bit
    //a opcode is the word at pc filled with 8 zeros, and
    //then the word at pc+1 is pasted behind that
    //Decode Opcodes and execute opcode
    //example:
    //memory[pc] = 0xA2
    //memory[pc+1] = 0xF0
    //0xA2 << 8 = 0xA200
    //0xA200 | 0xF0 = 0XA2F0
    //
    //// the opcodes and their oparations

    //Switch on the First 4 bits
    std::cout << "Opcode: " << std::hex << opcode << std::endl;
    switch (opcode & 0xF000) {


        case 0x0000:
            switch (opcode) {

                case 0x00E0:
                    //CLS, clean sreen
                    GS->cleanBoard();
                    PC += 2;
                    break;

                case 0x00EE:
                    // RET, return
                    if (SP == 0) {
                        std::cout << "PC < 0!!" << std::endl;
                        this->panic();
                    } else {
                        PC = Stack[SP];
                        SP--;
                    }
                    break;
            }
            break;

        case 0x1000:
            // JP, jump to NNN
            //Jump to Address in last 3 bytes NNN: Change PC to NNN (PC was 12 bit originaly)
            PC = opcode & 0x0FFF;
            if (PC > 4095) {
                std::cerr << "PC bigger then 0xFFF, DYING" << std::endl;
                this->panic();
            }
            break;

        case 0x2000:
            // CALL : Get into subroutine, push old PC to stack and set PC
            SP++;
            Stack[SP] = PC;
            PC = (opcode & 0x0FFF);
            break;

        case 0x3000:
            // SE, Skip if Equal, (Reg,RAM)
            // opcode 0x3xkk	Reg[x]				  ==		kk -> pc += 2
            if (Registers[(opcode >> 8 & 0x000F)] == (opcode & 0x00FF))
                PC += 2;
            PC += 2;
            break;

        case 0x4000:
            // SNE, Skip if nequal (Reg,Ram)
            // opcode 0x4xkk	Reg[x]				  !=		kk -> pc += 2
            if (Registers[(opcode >> 8 & 0x000F)] != (opcode & 0x00FF))
                PC += 2;
            PC += 2;
            break;

        case 0x5000:
            // SE, Skip if equal (Reg, Reg)
            // opcode 0x4xy0	Reg[x]				    == 		Reg[y]   -> pc += 2
            if (Registers[(opcode >> 8 & 0x000F)] == Registers[(opcode >> 4 & 0x000F)])
                PC += 2;
            PC += 2;
            break;

        case 0x6000:
            // LD, Load kk in Reg[x]
            // 0x6xkk: Set Register x to kk
            Registers[(opcode >> 8 & 0x000F)] = (opcode & 0x00FF);
            PC += 2;
            break;

        case 0x7000:
            // 7xkk - ADD, Reg[x] += kk
            Registers[(opcode >> 8 & 0x000F)] += (opcode & 0x00FF);
            PC += 2;
            break;

        case 0x8000:
            switch (opcode & 0x000F) {
                case 0:
                    // Set Vx = Vy
                    Registers[(opcode >> 8 & 0x000F)] = Registers[(opcode >> 4 & 0x000F)];
                    PC += 2;
                    break;

                case 1:
                    // Set Vx = Vx | Vy
                    Registers[(opcode >> 8 & 0x000F)] =
                            Registers[(opcode >> 8 & 0x000F)] | Registers[(opcode >> 4 & 0x000F)];
                    PC += 2;
                    break;

                case 2:
                    // Set Vx = Vx & Vy
                    Registers[(opcode >> 8 & 0x000F)] =
                            Registers[(opcode >> 8 & 0x000F)] & Registers[(opcode >> 4 & 0x000F)];
                    PC += 2;
                    break;

                case 3:
                    // Set Vx = Vx ^ Vy
                    Registers[(opcode >> 8 & 0x000F)] =
                            Registers[(opcode >> 8 & 0x000F)] ^ Registers[(opcode >> 4 & 0x000F)];
                    PC += 2;
                    break;

                case 4: {
                    //set Vx = Vx + Vy, set VF = carry
                    //use VF as
                    uint16_t temp = Registers[(opcode >> 8 & 0x000F)] + Registers[(opcode >> 4 & 0x000F)];
                    // Keep Lowest 8 bits
                    Registers[(opcode >> 8 & 0x000F)] = temp & 0x00FF;
                    // If Result was bigger then 255, VF = 1, otherwise 0
                    if(temp > 255)
                        VF = 1;

                    PC += 2;
                    break;
                }
                case 5:
                    //set Vx = Vx - Vy, set VF = Borrow
                    if (Registers[(opcode >> 8 & 0x000F)] >
                        Registers[(opcode >> 4 & 0x000F)])
                        VF = 1;
                    else
                        VF = 0;
                    Registers[(opcode >> 8 & 0x000F)] -= Registers[(opcode >> 4 & 0x000F)];
                    PC += 2;
                    break;

                case 6:
                    // SHR Vx {, Vy}
                    // Set Vx = Vx SHR 1.

                    if (Registers[(opcode >> 8 & 0x000F)] & 0x0001)
                        VF = 1;
                    else
                        VF = 0;
                    Registers[(opcode >> 8 & 0x000F)] = Registers[(opcode >> 8 & 0x0001)] >> 1;
                    PC += 2;
                    break;

                case 7:
                    //SUBN Vx, Vy
                    //Set Vx = Vy - Vx, set VF = NOT borrow.
                    if (Registers[(opcode >> 8 & 0x000F)] <
                        Registers[(opcode >> 4 & 0x000F)])
                        VF = 1;
                    else
                        VF = 0;
                    Registers[(opcode >> 8 & 0x000F)] =
                            Registers[(opcode >> 4 & 0x000F)] - Registers[(opcode >> 8 & 0x000F)];
                    PC += 2;
                    break;

                case 0xE:
                    // SHL Vx {, Vy}
                    // Set Vx = Vx SHL 1. IF Reg(x) MSB == 1
                    if (Registers[(opcode >> 8 & 0x000F)] >>  7)
                        VF = 1;
                    else
                        VF = 0;
                    Registers[(opcode >> 8 & 0x000F)] = Registers[(opcode >> 8 & 0x000F)] << 1;
                    PC += 2;
                    break;
            }

        case 0x9000:
            if (Registers[(opcode >> 8 & 0x000F)] != Registers[(opcode >> 4 & 0x000F)])
                PC += 2;
            PC += 2;
            break;

        case 0XA000:
            //LD I, addr
            I = (opcode & 0x0FFF);
            PC += 2;
            break;

        case 0xB000:
            // jmp V0, addr
            PC = Registers[0] + (opcode & 0x0FFF);
            break;

        case 0xC000:
//			RND Vx, byte
//			Set Vx = random byte AND kk.
            Registers[(opcode >> 8 & 0x000F)] = ((uint8_t) rand() & (uint8_t) (opcode & 0x00FF));
            PC += 2;
            break;

        case 0xD000: {
            uint8_t x = Registers[(opcode >> 8 & 0x000F)];
            uint8_t y = Registers[(opcode >> 4 & 0x000F)];
            uint8_t h = (opcode & 0x000F);
            VF = 0;

            //Draw X first, y second
            for (size_t j = 0; j < h; j++) {
                if (I + j > 4096) {
                    std::cerr << "Out Of memory!" << std::endl;
                    this->panic();
                }
                uint8_t pixel = RAM[I + j];

                if ((j + y) >= GS->getNy())
                    y = 0;

                for (uint8_t i = 0; i < 8; i++) {
                    if ((i + x) >= GS->getNx())
                        x = 0;

                    //Toggle bit, if bit is zeroed, zet VF (collision!)
                    if (screen[x + i][y + j] ^= (pixel & (1 << i)) == 0)
                        VF = 1;
                }
                // Reset X, in case we went out of screen in X
                x = Registers[(opcode >> 8 & 0x000F)];
            }
            PC += 2;
            break;
        }

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x9E:
                    //            Ex9E - SKP Vx
                    //            Skip next instruction if key with the value of Vx is pressed.
                    if (Registers[(opcode >> 8 & 0x000F)] & keyboard)
                        PC += 2;
                    PC += 2;
                    break;

                case 0xA1:
                    //            ExA1 - SKNP Vx
                    //            Skip next instruction if key with the value of Vx is NOT pressed.
                    if (Registers[(opcode >> 8 & 0x000F)] ^ keyboard)
                        PC += 2;
                    PC += 2;
                    break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07:
                    //Set Vx = timer
                    Registers[(opcode >> 8 & 0x000F)] = timer;
                    PC += 2;
                    break;

                case 0x0A:
                    // Wait For key, and store in Vx, by returning and NOT increasing PC, we essentially wait
                    if (!keyboard) {
                        std::cout << "Waiting For Input" << std::endl;
                        return;
                    }
                    else {
                        Registers[(opcode >> 8 & 0x000F)] = keyboard;
                        PC += 2;
                    }
                    break;

                case 0x15:
                    // Fx15 LD DT, Vx
                    // Set Delay Timer to Vx
                    timer = Registers[(opcode >> 8 & 0x000F)];
                    PC += 2;
                    break;

                case 0x18:
                    // Fx18 LD ST, Vx
                    // Set Sound Timer to Vx
                    soundTimer = Registers[(opcode >> 8 & 0x000F)];
                    PC += 2;
                    break;

                case 0x1E:
                    // Fx1E - Add I, Vx
                    // Set I to I + Vx
                    I += Registers[(opcode >> 8 & 0x000F)];
                    PC += 2;
                    break;

                case 0x29:
                    // Fx29 - LD F, Vx
                    // Set I to address of the charsprite in Vx
//                    std::cout << "Printing Sprite on Screen! "<< (char)Registers[(opcode >> 8 & 0x000F)] << std::endl;
                    I = RAM[Registers[(opcode >> 8 & 0x000F)] * 5];
                    PC += 2;
                    break;

                case 0x33:
                    // Fx33 - LD B, Vx
                    //Store BCD representation of Vx in memory location I, I+1, I+2
                    RAM[I] = Registers[(opcode >> 8 & 0x000F)] / 100;
                    RAM[I + 1] = (Registers[(opcode >> 8 & 0x000F)] / 10) % 10;
                    RAM[I + 2] = (Registers[(opcode >> 8 & 0x000F)] % 100) / 100;
                    PC += 2;
                    break;

                case 0x55:
                    //Fx55 - LD [I], Vx
                    //Store All Registers in Ram at I
                    for (uint8_t i = 0; i <= (opcode >> 8 & 0x000F); i++) {
                        RAM[I + i] = Registers[i];
                    }
                    PC += 2;
                    break;

                case 0x65:
                    //Fx65 - LD Vx, [I]
                    // Load all Registers in Ram from I
                    for (uint8_t i = 0; i <= (opcode >> 8 & 0x000F); i++) {
                        Registers[i] = RAM[I + i];
                    }
                    PC += 2;

                    break;
            }
            break;


    }

    this->panic();
}

void chip_8_emu::setKeyPress(SDL_Keycode key) {
    switch (key) {
        case SDLK_KP_1:
            keyboard &= 0x01;
            break;
        case SDLK_KP_2:
            keyboard &= 0x02;
            break;
        case SDLK_KP_3:
            keyboard &= 0x03;
            break;
        case SDLK_KP_4:
            keyboard &= 0x04;
            break;
        case SDLK_KP_5:
            keyboard &= 0x05;
            break;
        case SDLK_KP_6:
            keyboard &= 0x06;
            break;
        case SDLK_KP_7:
            keyboard &= 0x07;
            break;
        case SDLK_KP_8:
            keyboard &= 0x08;
            break;
        case SDLK_KP_9:
            keyboard &= 0x09;
            break;
        case SDLK_KP_0:
            keyboard &= 0x0A;
            break;
    }
}

void chip_8_emu::releaseKey(SDL_Keycode key) {
    switch (key) {
        case SDLK_KP_1:
            keyboard &= 0x01;
            break;
        case SDLK_KP_2:
            keyboard &= 0x02;
            break;
        case SDLK_KP_3:
            keyboard &= 0x03;
            break;
        case SDLK_KP_4:
            keyboard &= 0x04;
            break;
        case SDLK_KP_5:
            keyboard &= 0x05;
            break;
        case SDLK_KP_6:
            keyboard &= 0x06;
            break;
        case SDLK_KP_7:
            keyboard &= 0x07;
            break;
        case SDLK_KP_8:
            keyboard &= 0x08;
            break;
        case SDLK_KP_9:
            keyboard &= 0x09;
            break;
        case SDLK_KP_0:
            keyboard &= 0x0A;
            break;
    }
}

int chip_8_emu::loadRom(const char *file) {
    std::cout << "\t CHIP_8 Loading File: " << file << std::endl;

    uint8_t buffer[3584];

    memset(&buffer, 0, sizeof(uint8_t) * 3584);

    FILE *f;
    f = fopen(file, "rb");
    uint8_t byte;

    if(f == nullptr){
        std::cout << "Could Not Open File: " << file << std::endl;
        return 1;
    }
    else {
        for(size_t i = 0; (byte = fgetc(f)) != EOF && i < 3584; i++){
            buffer[i] = byte;
        }
        fclose(f);

        for(size_t i = 0; i < 3584; i++){
            RAM[i + 512] = buffer[i];
        }
    }

    std::cout << "\t CHIP_8 Done Loading!" << std::endl;
    return 0;
}

void chip_8_emu::panic() {

    std::cout << "---- PANIC ----" << std::endl;
    std::cout << "RegDump: " << std::endl;
    for (size_t i = 0; i < 16; i++)
        std::cout << 'V' << i << ":" << (int) Registers[i] << '\t';
    std::cout << '\n';

    std::cout << "Stack in HEX: " << std::endl;
    for (size_t i = 0; i < 16; i++){
        std::cout << std::hex << Stack[i] << '\t';
    }
    std::cout << '\n';

    std::cout << "I:" << I << '\t';
    std::cout << "PC:" << PC << '\t';
    std::cout << "VF:" << VF << '\t';
    std::cout << "SP:" << SP << '\n';

    std::cout <<std::endl;
    std::cout << "Emulator Stopped!" << std::endl;

    //this->STOP = true;


}