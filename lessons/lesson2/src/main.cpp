#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "res_path.h"
#include "cleanup.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Log an SDL error with some error message to the output stream
// of our choice.
// @param os The output stream to write the message to
// @param msg The error message to write, format will be
// "msg error: SDL_GetError()"
void logSDLError(std::ostream &os, const std::string &msg)
{
    os << msg << SDL_GetError() << std::endl;
}

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren)
{
    SDL_Texture *texture = nullptr;
    SDL_Surface *loadedImage = SDL_LoadBMP(file.c_str());

    if (loadedImage != nullptr)
    {
        texture = SDL_CreateTextureFromSurface(ren, loadedImage);
        SDL_FreeSurface(loadedImage);

        if (texture == nullptr)
        {
            logSDLError(std::cout, "CreateTextureFromSurface");
        }
    }
    else
    {
        logSDLError(std::cout, "LoadBMP");
    }
    
    return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
{
    // Setup destination rectangle and place it at desired (x,y) location
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;

    // Get texture dimensions
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, NULL, &dst);
}

int main(int argc, char **argv)
{
    const int SDL_RENDERER_FIRST_AVAILABLE_DRIVER = -1;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_init error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == nullptr)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, SDL_RENDERER_FIRST_AVAILABLE_DRIVER, SDL_RENDERER_ACCELERATED);
    if (ren == nullptr)
    {
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::string imagePath = get_resource_path("lesson2") + "hello.bmp";
    SDL_Surface *bmp = SDL_LoadBMP(imagePath.c_str());
    if (bmp == nullptr)
    {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);
    if (tex == nullptr)
    {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Render
    for (int i=0; i < 3; ++i)
    {
        // Clear the renderer
        SDL_RenderClear(ren);

        // Draw the texture
        SDL_RenderCopy(ren, tex, NULL, NULL);

        // Update the screen
        SDL_RenderPresent(ren);

        // Delay the closing of the window so we can see stuff
        SDL_Delay(1000);
    }

    cleanup(tex, ren, win);
    SDL_Quit();
    return 0;
}