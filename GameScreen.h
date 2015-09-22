//
// Created by lucas on 7/14/15.
//

#ifndef GAMEOFLIDE_SDL2_GAMEBOARD_H
#define GAMEOFLIDE_SDL2_GAMEBOARD_H


#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

class GameScreen {

    const size_t Nx, Ny;
    const int tileWidth, tileHeight;
    SDL_Rect ***tiles;
    uint8_t **states;
    bool Grid;

public:

    GameScreen(size_t Nx, size_t Ny, int tileWidth, int tileHeight);

    void cleanBoard();

    void showGrid();

    void drawBoard(SDL_Renderer *ren);

    uint8_t ** getStates(){ return states;  };
    size_t getNx(){ return Nx; };
    size_t getNy(){ return Ny; };

    size_t getTileWidth(){ return tileWidth; };
    size_t getTileHeight(){ return tileHeight; };

    ~GameScreen();

};


#endif //GAMEOFLIDE_SDL2_GAMEBOARD_H
