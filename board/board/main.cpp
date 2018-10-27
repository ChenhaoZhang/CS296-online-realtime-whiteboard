//
//  main.cpp
//  board
//
//  Created by Felix Yang on 10/26/18.
//  Copyright © 2018 Felix Yang. All rights reserved.
//

//the idea and source code come from following website
//https://bitbucket.org/dandago/gigilabs/src/ba730d3aaba9e1cd7d7864d7dc98a2e694fcaddc/Sdl2PixelDrawing/Sdl2PixelDrawing/main.cpp?at=master&fileviewer=file-view-default

#include <iostream>
#include <SDL2/SDL.h>

#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 1000

int main(int argc, char ** argv)
{
    bool leftMouseButtonDown = false;
    bool quit = false;
    SDL_Event event;
    
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Window * window = SDL_CreateWindow("Board",
                                           SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture * texture = SDL_CreateTexture(renderer,
                                              SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WINDOW_WIDTH, WINDOW_HEIGHT);
    Uint32 * pixels = new Uint32[WINDOW_WIDTH * WINDOW_HEIGHT];
    
    memset(pixels, 255, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(Uint32));
    
    while (!quit)
    {
        SDL_UpdateTexture(texture, NULL, pixels, WINDOW_WIDTH * sizeof(Uint32));
        SDL_Delay(1);
        SDL_WaitEvent(&event);
        
        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                    leftMouseButtonDown = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                    leftMouseButtonDown = true;
            case SDL_MOUSEMOTION:
                if (leftMouseButtonDown)
                {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;
                    pixels[mouseY * WINDOW_WIDTH + mouseX] = 0;
                }
                break;
        }
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    
    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
