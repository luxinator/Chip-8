//
// Created by lucas on 7/14/15.
//

#include <iostream>
#include "GameScreen.h"



GameScreen::GameScreen(size_t Nx, size_t Ny, int tileWidth, int tileHeight) : Nx(Ny), Ny(Nx), tileWidth(tileWidth), tileHeight(tileHeight) {


    //  Allocate 3D Array
//    int *allElements = malloc(x * y * z * sizeof(int));
//    int ***array3D = malloc(x * sizeof(int **));
//
//    for(i = 0; i < x; i++)
//    {
//        array3D[i] = malloc(y * sizeof(int *));
//
//        for(j = 0; j < y; j++)
//        {
//            array3D[i][j] = allElements + (i * y * z) + (j * z);
//        }
//    }

    //  Allocate 2D Array
//    int *allElements = malloc(x * y * sizeof(int));
//    int **array2D = malloc(x * sizeof(int **));
//
//    for(i = 0; i < x; i++)
//    {
//         array2D[i] = allElements + (i * y);
//      }

    std::cout << "Initializing Sreen" << std::endl;

    this->Grid = false;

    uint8_t * StatesBlock = new uint8_t[this->Nx*this->Ny];
    this->states = new uint8_t*[this->Nx];
    for(size_t i = 0; i < this->Nx; i++){
        this->states[i] = StatesBlock + i * this->Ny;
    }

    SDL_Rect ** tilesBlock = new SDL_Rect*[this->Nx*this->Ny];
    this->tiles = new SDL_Rect**[this->Nx];
    for(size_t i = 0; i < this->Nx; i++){
        this->tiles[i] = tilesBlock + i * this->Ny;
    }


    for (size_t i = 0; i < this->Nx; i++) {
        for (size_t j = 0; j < this->Ny; j++) {
            this->states[i][j] = (uint8_t) 0;

            this->tiles[i][j] = new SDL_Rect;
            this->tiles[i][j]->x = (int) i * tileWidth;
            this->tiles[i][j]->y = (int) j * tileHeight;

            this->tiles[i][j]->h = tileHeight;
            this->tiles[i][j]->w = tileWidth;
            std::cout << this->tiles[i][j]->x << '\t' << this->tiles[i][j]->y << '\t';
            std::cout << this->tiles[i][j]->w << '\t' << this->tiles[i][j]->h << '\t' << (int)this->states[i][j] << std::endl;
        }
    }
    std::cout << "Done!" << std::endl;

}

void GameScreen::showGrid() {
    if (this->Grid)
        this->Grid = false;
    else
        this->Grid = true;
}


void GameScreen::drawBoard(SDL_Renderer * ren) {

   //Draw everything:
    for (size_t i = 0; i < Nx; i++) {
        for (size_t j = 0; j < Ny; j++) {
//            std::cout << "Drawing [" << i << ',' << j << ']' <<  (int)states[i][j] << std::endl;
            if (this->states[i][j] == (uint8_t) 1) {
                SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
                SDL_RenderFillRect(ren, this->tiles[i][j]);
            }
        }
    }
    //Draw Grid Lines
    if (this->Grid) {
        SDL_SetRenderDrawColor(ren, 0, 125, 125, 75);

        for (size_t i = 1; i < Nx - 1; i++) {
            SDL_RenderDrawLine(ren, i * this->tileWidth, 0,
                               i * this->tileWidth, Ny * tileWidth);
        }
        for (size_t j = 1; j < Ny - 1; j++) {
            SDL_RenderDrawLine(ren, 0, j * this->tileHeight,
                               this->tileWidth * Nx, this->tileHeight * j);
        }

    }
}

void GameScreen::cleanBoard() {
    std::cout << "Clearing Screen" <<std::endl;
    for (size_t i = 0; i < Nx; i++) {
        for (size_t j = 0; j < Ny ; j++) {
            this->states[i][j] = (char) 0;
        }
    }
}

GameScreen::~GameScreen(){
    std::cout << "Cleaning up Screen" << std::endl;
    for (size_t i = 0; i < Nx; i++) {
        for (size_t j = 0; j < Ny; j++) {
            // std::cout << "Deleting [" << i << ',' << j << ']' << std::endl;
            delete this->tiles[i][j];
            }
        }

    delete[] this->tiles;
    delete[] this->states;

}