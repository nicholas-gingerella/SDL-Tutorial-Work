#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "res_path.h"
#include "cleanup.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int PAUSE_TIME_IN_SECONDS = 10;
const int MILLISECONDS_IN_SECONDS = 1000;
const int SDL_RENDERER_FIRST_AVAILABLE_DRIVER = -1;

// Log an SDL error with some error message to the output stream
// of our choice.
// @param os The output stream to write the message to
// @param msg The error message to write, format will be
// "msg error: SDL_GetError()"
void logSDLError(std::ostream &os, const std::string &msg)
{
    os << msg << SDL_GetError() << std::endl;
}

// Load texture from file
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren)
{
    SDL_Texture *texture = nullptr;
    SDL_Surface *loaded_image = SDL_LoadBMP(file.c_str());

    if (loaded_image != nullptr)
    {
        texture = SDL_CreateTextureFromSurface(ren, loaded_image);
        SDL_FreeSurface(loaded_image);

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


// Render a texture at a position on the screen
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
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        logSDLError(std::cout, "SDL_init error");
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        logSDLError(std::cout, "SDL_CreateWindow Error");
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window,
    SDL_RENDERER_FIRST_AVAILABLE_DRIVER,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
    {
        cleanup(window);
        logSDLError(std::cout, "SDL_CreateRenderer Error");
        SDL_Quit();
        return 1;
    }


    // Load images
    const std::string resource_path = get_resource_path("lesson2");
    SDL_Texture *bg_texture = loadTexture(resource_path + "background.bmp", renderer);
    SDL_Texture *img_texture = loadTexture(resource_path + "image.bmp", renderer);
    if ( (bg_texture == nullptr) || (img_texture == nullptr) )
    {
        cleanup(bg_texture, img_texture, renderer, window);
        SDL_Quit();
        return 1;
    }

    // Render tiled background to window
    int bg_width, bg_height;
    SDL_QueryTexture(bg_texture, NULL, NULL, &bg_width, &bg_height);
    for (int bg_pos_x = 0; bg_pos_x < SCREEN_WIDTH; bg_pos_x += bg_width)
    {
        for (int bg_pos_y = 0; bg_pos_y < SCREEN_HEIGHT; bg_pos_y += bg_height)
        {
            renderTexture(bg_texture, renderer, bg_pos_x, bg_pos_y);
        }
    }

    // Render image to center of window
    int img_width, img_height;
    SDL_QueryTexture(img_texture, NULL, NULL, &img_width, &img_height);
    int img_pos_x = (SCREEN_WIDTH / 2) - (img_width / 2);
    int img_pos_y = (SCREEN_HEIGHT / 2) - (img_height/ 2);
    renderTexture(img_texture, renderer, img_pos_x, img_pos_y);

    // Render current contents and pause
    SDL_RenderPresent(renderer);
    SDL_Delay(PAUSE_TIME_IN_SECONDS * MILLISECONDS_IN_SECONDS);

    cleanup(bg_texture, img_texture, renderer, window);
    SDL_Quit();
    return 0;
}