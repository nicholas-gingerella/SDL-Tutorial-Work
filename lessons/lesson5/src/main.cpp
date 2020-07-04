#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "res_path.h"
#include "cleanup.h"

const int clipWidth = 100;
const int clipHeight = 100;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 40;
const int PAUSE_TIME_IN_SECONDS = 10;
const int MILLISECONDS_IN_SECONDS = 1000;
const int SDL_RENDERER_FIRST_AVAILABLE_DRIVER = -1;
const std::string CURRENT_LESSON = "lesson5";

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
// @param dst The destination rectangle to render th texture to
// @param clip The sub-section of the texture to draw (clipping rect)
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst, SDL_Rect *clip = nullptr)
{
	SDL_RenderCopy(ren, tex, clip, &dst);
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

// Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
// the texture's width and height and taking a clip of the texture if
// if a clip rectangle is passed. If a clip is present, the clip's width
// and height will be used in place of the texture's dimensions
// @param tex The source texture we want to draw
// @param ren The renderer we want to draw to
// @param x The x coordinate to draw to
// @param y The y coordinate to draw to
// @param clip The sub-section of the texture to draw (clipping rect)
//             default of nullptr draws the entire texture
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip = nullptr)
{
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
    if (clip != nullptr)
    {
        dst.w = clip->w;
        dst.h = clip->h;
    }
    else
    {
        SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    }

    renderTexture(tex, ren, dst, clip);    
}


bool setupSDL(SDL_Window *window, SDL_Renderer *renderer)
{


    return true;
}

int main(int argc, char **argv)
{
    // Init SDL_image to avoid delay on first image load.
    // IMG_Init returns the currently initialized image loaders.
    // A logical AND with the image loader flag we are checking
    // allows us to verify the image loader was successfully 
    // initialized
    if ( (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG )
    {
        logSDLError(std::cout, "IMG_Init");
        return false;
    }

    SDL_Window *window = SDL_CreateWindow("Lesson5 - Sprite Sheets", 
        100, 
        100, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        logSDLError(std::cout, "SDL_CreateWindow Error");
        SDL_Quit();
        return false;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window,
        SDL_RENDERER_FIRST_AVAILABLE_DRIVER,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


    // Load image
    const std::string resource_path = get_resource_path(CURRENT_LESSON);
    SDL_Texture *tex_img = loadTexture(resource_path + "image.png", renderer);
    if ( tex_img == nullptr )
    {
        cleanup(tex_img, renderer, window);
        SDL_Quit();
        return 1;
    }

    // Calculate position for center of screen
    int img_width, img_height;
    SDL_QueryTexture(tex_img, NULL, NULL, &img_width, &img_height);

    // Image is clipped to size, so make sure to use the clip size
    // when calculating the center position for the image on screen
    int img_pos_x = (SCREEN_WIDTH / 2) - (clipWidth / 2);
    int img_pos_y = (SCREEN_HEIGHT / 2) - (clipHeight/ 2);

    // Setup the clips for our image
    const int num_sprite_rows = 2;
    const int num_sprite_cols = 2;
    SDL_Rect clips[4];
    for ( int i = 0; i < 4; ++i)
    {
        clips[i].x = (i / num_sprite_cols) * clipWidth;
        clips[i].y = (i % num_sprite_rows) * clipHeight;
        clips[i].w = clipWidth;
        clips[i].h = clipHeight;
    }
    // Specify a default clip to start with
    int useClip =  0;

    // Setup main loop
    SDL_Event event;
    bool quit = false;
    while(!quit)
    {
        // Read user input
        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_1:
                        useClip = 0;
                        break;
                    case SDLK_2:
                        useClip = 1;
                        break;
                    case SDLK_3:
                        useClip = 2;
                        break;
                    case SDLK_4:
                        useClip = 3;
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    default:
                        break;
                }
            }
        }

        // Render scene
        SDL_RenderClear(renderer);
        renderTexture(tex_img, renderer, img_pos_x, img_pos_y, &clips[useClip]);
        SDL_RenderPresent(renderer);
    }

    cleanup(tex_img, renderer, window);
    SDL_Quit();
    return 0;
}