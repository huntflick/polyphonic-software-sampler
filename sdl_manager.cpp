#include "sdl_manager.hpp"

SDL_Manager& SDL_Manager::sdl() {
    static SDL_Manager instance;
    return instance;
}

SDL_Manager::SDL_Manager() {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetSwapInterval(-1);

    for (int i = 0; i < MAX_WINDOWS; ++i) {
        windows[i] = nullptr;
        buffers[i] = nullptr;
        update[i] = false;
    }
    
    spawnWindow("OpenGL Context", 720, 600, 0);
}

SDL_Manager::~SDL_Manager() {
    closeWindow(SDL_GetWindowID(windows[0]));
    SDL_Quit();
}

void SDL_Manager::spawnWindow(const std::string title, int width, int height, bool resizeable) {
    if (count >= MAX_WINDOWS) return;
    
    const std::string holder = title + std::to_string(count);
    const char *charTitle = holder.c_str();
    SDL_Window *window = nullptr;
    if (count == 0) window = SDL_CreateWindow(charTitle, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS);
    else window = SDL_CreateWindow(charTitle, width, height, SDL_WINDOW_INPUT_FOCUS);
    if (window) {
        if (count == 0) {
            context = SDL_GL_CreateContext(window);
            if (!context) {
                std::cerr << SDL_GetError();
                exit(1);
            }
            glClearColor(1.0f, 0.8f, 1.0f, 1.0f);
            bool safe = SDL_GL_MakeCurrent(window, context);
            if (!safe) {
                std::cerr << SDL_GetError();
                exit(1);
            }
            windows[count] = window;
            update[count] = true;
            count++;
        }
        else {
            SDL_Surface *buffer = SDL_GetWindowSurface(window);
            if (buffer) {
                windows[count] = window;
                buffers[count] = buffer;
                update[count] = true;
                count++;
            }
            else {
                const char *error = SDL_GetError();
                std::cerr << error;
                SDL_Manager::~SDL_Manager();
                exit(1);
            }
            const SDL_PixelFormatDetails *format =  SDL_GetPixelFormatDetails(buffer->format);
            SDL_FillSurfaceRect(buffer, NULL, SDL_MapRGB(format, NULL, 0x11, 0xCC, 0xFF));
            
        }
    }
    else {
        const char *error = SDL_GetError();
        std::cerr << error;
        SDL_Manager::~SDL_Manager();
        exit(1);
    }
}

void SDL_Manager::updateWindows() {
    for (int i = 0; i < count; ++i) {
        if (windows[i] && update[i]) {
            bool safe = true;
            if (i != 0) {
                safe = SDL_UpdateWindowSurface(windows[i]);
                update[i] = false;
                if (!safe) {
                    const char *error = SDL_GetError();
                    std::cerr << error;
                    SDL_Manager::~SDL_Manager();
                    exit(1);
                }
            }
            else {
                // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // glFlush();
                SDL_GL_SwapWindow(windows[i]);
            }
        }
    }
}

void SDL_Manager::closeWindow(std::uint32_t id) {   // delete window, memcpy rest of array over one spot, make sure end of old data is reset
    if (id == SDL_GetWindowID(windows[0])) {
        for (int i = 1; i < count; ++i) {
            if (windows[i]) {
                SDL_DestroyWindow(windows[i]);
                windows[i] = nullptr;
                buffers[i] = nullptr;
                count--;
            }
        }
        if (windows[0]) {
            SDL_GL_DestroyContext(context);
            SDL_DestroyWindow(windows[0]);
            windows[0] = nullptr;
            buffers[0] = nullptr;
            count--;
            SDL_Event ev;
            ev.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&ev);
        }
    }
    else {
        for (int i = 1; i < count; ++i) {
            if (SDL_GetWindowID(windows[i]) == id) {
                SDL_DestroyWindow(windows[i]);
                windows[i] = windows[count-1];
                buffers[i] = buffers[count-1];
                update[i] = update[count-1];
                windows[count-1] = nullptr;
                buffers[count-1] = nullptr;
                update[count-1] = false;
                count--;
                return;
            }
        }
    }
}