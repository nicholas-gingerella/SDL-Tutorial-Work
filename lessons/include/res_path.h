#ifndef RES_PATH_H
#define RES_PATH_H

#include <iostream>
#include <string>
#include <SDL2/SDL.h>

// Get path for resources located int res/sub_dir
// Project directory structure assumed to be the 
// following:
// bin/
//   executable
// res/
//   lesson1/
//   lesson2/
//
// Paths returned will be lessons/res/sub_dir
std::string get_resource_path(const std::string &subDir = "")
{
    // Choose path separator based one platform (Windows vs Linux)
    #ifdef _WIN32
        const char PATH_SEP = '\\';
    #else
        const char PATH_SEP = '/';
    #endif

    // will hold resource path: lessons/res/
    // declared static so SDL_GetBasePath only needs to be called
    // once, after which the value of this variable will persist
    // for entirety of the program run
    static std::string baseRes;
    if (baseRes.empty())
    {
        // SDL_GetBasePath() returns NULL if it fails to retrieve
        // a path
        char *basePath = SDL_GetBasePath();
        if (basePath)
        {
            baseRes = basePath;
            SDL_free(basePath);
        }
        else
        {
            std::cerr << "Error getting resource path" << SDL_GetError() << std::endl;
            return "";
        }

        // Replace the last "bin/" with "res/" to get the resource path
        size_t pos = baseRes.rfind("bin");
        baseRes = baseRes.substr(0, pos) + "res" + PATH_SEP;
    }

    // If a subdirectory is specified, append it to the base path
    // otherwise, just return the base path
    return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}
#endif