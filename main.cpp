#include <iostream>
#include <SDL2/SDL.h>

#include "GameScreen.h"
#include "Emulator.h"

const int SCREEN_WIDTH = 640; const int SCREEN_HEIGHT = 480;
const int nrOfTilesX = 64, nrOfTilesY = 48;
const int tileWidth = 10; //nr of Pixels
const Uint32 REFRESH_RATE = 100;
bool quit;

const int border = 0;

int main() {

    SDL_Window *window;
    quit = false;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize!\n SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    window = SDL_CreateWindow("GameOfLife", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cout << "SDL could not initialize!\n SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    //SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        std::cout << "SDL could not initialize!\n SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }


    GameScreen *GS = new GameScreen(nrOfTilesX, nrOfTilesY, tileWidth, tileWidth);
    if(GS == nullptr){
        std::cout << "Screen Did not Initialize" << std::endl;
        return -1;
    }
    GS->drawBoard(renderer);

    //View of the board
    //SDL_Rect boardView    = {0, 0, (SCREEN_WIDTH / tileWidth) * tileWidth - border, (SCREEN_HEIGHT/ tileWidth) * tileWidth - border};

    //Where to draw on screen, if the w and h of these are the same, then there is no scaling.
    //SDL_Rect onScreenView = {0, 0, (SCREEN_WIDTH / tileWidth) * tileWidth - border, (SCREEN_HEIGHT/ tileWidth) * tileWidth - border};
    // Enter Game Loop Here is Where the Magic Happens
    SDL_Event event;

    Uint32 timer = 0;
    Uint32 timeToRefresh;

    SDL_Texture * boardText = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_TARGET,
                      nrOfTilesX*tileWidth,  nrOfTilesY*tileWidth);

    int mouseX, mouseY;
    bool mouseLeftDown = false;
    bool mouseRightDown = false;

    bool pause = true;
    bool step = false;

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);


    chip_8_emu * chip8 = new chip_8_emu(GS);

    if(chip8->loadRom("/home/lucas/Dropbox/Hobby/Lux_Chip_8/c8games/C8pic.ch8") != 0){
        return -1;
    }

    std::cout << "Starting Main Loop" << std::endl;
    while (!quit) {

        //Events
        while (SDL_PollEvent(&event) != 0) {

            if (event.type == SDL_QUIT)
                quit = true;
            else if (event.type == SDL_KEYDOWN) {

                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_SPACE:
                        if (pause)
                            pause = false;
                        else
                            pause = true;
                        break;

                    case SDLK_t:
                        if (step)
                            step = false;
                        else
                            step = true;
                        break;


                    case SDLK_g:
                        GS->showGrid();
                        break;


                    default:
                        chip8->setKeyPress(event.key.keysym.sym);
                        break;
                }
            } else if (event.type == SDL_KEYUP) {

                switch (event.key.keysym.sym) {
                    default:
                        chip8->releaseKey(event.key.keysym.sym);
                        break;
                }
            }
        }
        

        //Logic
        if (!pause && SDL_GetTicks() - timer > REFRESH_RATE) {
            //std::cout << "TICK" << std::endl;
            timeToRefresh = timer - SDL_GetTicks();
            timer = SDL_GetTicks();
        } else if (pause && step) {
            std::cout << "USER TICK" << std::endl;
            step = false;
        }

        //Do Emulator Tick:
        if(!chip8->STOP)
            chip8->tick();

        // Render to boardTexture
        SDL_SetRenderTarget(renderer, boardText);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        // Clean the Renderer
        SDL_RenderClear(renderer);

        GS->drawBoard(renderer);

        //Set back to rendering on Screen
        SDL_SetRenderTarget(renderer, NULL);

        SDL_RenderCopy(renderer, boardText, NULL, NULL);

        SDL_RenderPresent(renderer);

    }


    delete GS;
    std::cout << "Cleaning up SDL" << std::endl;
    SDL_DestroyWindow(window);

    SDL_Quit();

    std::cout << "Quiting" << std::endl;
    return 0;
}