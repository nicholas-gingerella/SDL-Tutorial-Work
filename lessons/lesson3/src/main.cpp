#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "res_path.h"
#include "cleanup.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 40;
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

// Loads an image into a texture on the rendering device
// @param file The image file to load
// @param ren The renderer to load the texture onto
// @return the loaded texture, or nullptr if something went wrong
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren)
{
    SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
    if (texture == nullptr)
    {
        logSDLError(std::cout, "LoadTexture");
    }
    return texture;
}


// Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
// width and height
// @param tex The source texture we want to draw
// @param ren The renderer we want to draw to
// @param x The x coordinate to draw to
// @param y The y coordinate to draw to
// @param w The width of the texture to draw
// @param h The height of the texture to draw
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h){
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}


// Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
// the texture's width and height
// @param tex The source texture we want to draw
// @param ren The renderer we want to draw to
// @param x The x coordinate to draw to
// @param y The y coordinate to draw to
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
{
    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    renderTexture(tex, ren, x, y, w, h);
}


int main(int argc, char **argv)
{
    if ( SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        logSDLError(std::cout, "SDL_init error");
        return 1;
    }

    // Init SDL_image to avoid delay on first image load.
    // IMG_Init returns the currently initialized image loaders.
    // A logical AND with the image loader flag we are checking
    // allows us to verify the image loader was successfully 
    // initialized
    if ( (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG )
    {
        logSDLError(std::cout, "IMG_Init");
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Lesson3 - SDL_IMG", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
    const std::string resource_path = get_resource_path("lesson3");
    SDL_Texture *bg_texture = loadTexture(resource_path + "background.png", renderer);
    SDL_Texture *img_texture = loadTexture(resource_path + "image.png", renderer);
    if ( (bg_texture == nullptr) || (img_texture == nullptr) )
    {
        cleanup(bg_texture, img_texture, renderer, window);
        SDL_Quit();
        return 1;
    }

    // Render tiled background to window (tiles are square)
    int num_tiles_x = SCREEN_WIDTH / TILE_SIZE;
    int num_tiles_y = SCREEN_HEIGHT / TILE_SIZE;
    for (int i = 0; i < num_tiles_x * num_tiles_y; ++i)
    {
        // Calculate tile positions. Tiles drawn left-to-right,
        // row-by-row. X-coordinate repeats every complete row
        // (num tiles along width of screen), and the Y-coordinate
        // increases for each row (every multiple of num_tiles_x)
        int x = i % num_tiles_x;
        int y = i / num_tiles_x;
        renderTexture(bg_texture, renderer, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
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