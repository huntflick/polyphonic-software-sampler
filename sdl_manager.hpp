#ifndef __SDL_MANAGER_HPP__
#define __SDL_MANAGER_HPP__
#include <SDL3/SDL.h>
// #include <SDL3/SDL_opengl.h>
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <string>
#include <iostream>

const int MAX_WINDOWS = 20;
class SDL_Manager {
    private:
        SDL_Surface *buffers[MAX_WINDOWS];
        SDL_Window *windows[MAX_WINDOWS];
        bool update[MAX_WINDOWS];
        SDL_GLContext context;
        size_t count;
        SDL_Manager();
        ~SDL_Manager();
        
        SDL_Manager(const SDL_Manager&) = delete;
        SDL_Manager& operator=(const SDL_Manager&) = delete;
    
    public:
        static SDL_Manager& sdl();
        void spawnWindow(const std::string, int, int, bool);
        void updateWindows();
        void closeWindow(std::uint32_t);
        
};

#endif