#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "res_path.h"
#include "cleanup.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 40;
const int PAUSE_TIME_IN_SECONDS = 10;
const int MILLISECONDS_IN_SECONDS = 1000;
const int SDL_RENDERER_FIRST_AVAILABLE_DRIVER = -1;
const std::string CURRENT_LESSON = "lesson6";
const std::string WINDOW_TITLE = "Lesson 6 - Fonts";

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
// void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
// {
//     int w, h;
//     SDL_QueryTexture(tex, NULL, NULL, &w, &h);
//     renderTexture(tex, ren, x, y, w, h);
// }

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

// Render the message we want to display to a texture for drawing
// @param message The message we want to display
// @param fontFile The font we want to use to render the text
// @param color The color we want the text to be
// @param fontSize The size we want the font to be
// @param renderer The renderer to load the texture in
// @return An SDL_Texture containing the rendered message, or nullptr if something went wrong
SDL_Texture* renderText(const std::string &message, const std::string &fontFile,
	SDL_Color color, int fontSize, SDL_Renderer *renderer)
{
	// Open the font
	TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (font == nullptr)
    {
		logSDLError(std::cout, "TTF_OpenFont");
		return nullptr;
	}

	// We need to first render to a surface as that's what TTF_RenderText
	// returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
	if (surf == nullptr)
    {
		TTF_CloseFont(font);
		logSDLError(std::cout, "TTF_RenderText");
		return nullptr;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr)
    {
		logSDLError(std::cout, "CreateTexture");
	}

	// Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	return texture;
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

    if (TTF_Init() != 0)
    {
        logSDLError(std::cout, "TTF_Init");
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE.c_str(), 
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


    // Load text
    const std::string resource_path = get_resource_path(CURRENT_LESSON);
    SDL_Color color = {255, 255, 255, 255};
    SDL_Texture *tex_img = renderText("TTF fonts are cool!",
        resource_path + "sample.ttf",
        color,
        64,
        renderer);
    if ( tex_img == nullptr )
    {
        cleanup(renderer, window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Calculate position for center of screen
    int img_width, img_height;
    SDL_QueryTexture(tex_img, NULL, NULL, &img_width, &img_height);

    // Image is clipped to size, so make sure to use the clip size
    // when calculating the center position for the image on screen
    int img_pos_x = (SCREEN_WIDTH / 2) - (img_width / 2);
    int img_pos_y = (SCREEN_HEIGHT / 2) - (img_height/ 2);

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
        renderTexture(tex_img, renderer, img_pos_x, img_pos_y);
        SDL_RenderPresent(renderer);
    }

    cleanup(tex_img, renderer, window);
    SDL_Quit();
    return 0;
}