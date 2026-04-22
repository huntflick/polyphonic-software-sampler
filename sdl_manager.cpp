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
    TTF_Init();
    this->font = TTF_OpenFont("/System/Library/Fonts/NewYork.ttf", 16);
    for (int i = 0; i < MAX_WINDOWS; ++i) {
        windows[i] = nullptr;
        buffers[i] = nullptr;
        update[i] = false;
    }
    spawnWindow("Audio Interface", 300, 300, 0);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSDL3_InitForOpenGL(windows[0], this->context);
    ImGui_ImplOpenGL3_Init("#version 410");
}

SDL_Manager::~SDL_Manager() {
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
    closeWindow(SDL_GetWindowID(windows[0]));
    SDL_Quit();
}

void SDL_Manager::spawnWindow(const std::string title, int width, int height, bool resizeable) {
    if (count >= 2) return;
    const char *charTitle = title.c_str();
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

void SDL_Manager::updateWindows(int octaveOffset) {
    for (int i = 0; i < count; ++i) {
        if (windows[i]) {
            bool safe = true;
            if (i != 0) {
                drawPiano(octaveOffset);
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
            if (ImGui::GetCurrentContext()) {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplSDL3_Shutdown();
                ImGui::DestroyContext();
            }
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

void SDL_Manager::drawPiano(int octaveOffset) {
    if (1 >= count || !buffers[1]) {
        return;
    }
    const char* whiteKeyLabels[] = { "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'" };
    const char* blackKeyLabels[] = {"W", "E", "T", "Y", "U", "O", "P" };
    SDL_Surface* surface = buffers[1];
    const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
    SDL_FillSurfaceRect(surface, NULL, SDL_MapRGB(format, NULL, 50, 50, 50));
    int keyWidth = surface->w / 11;
    int blackKeyID = 0;
    for (int i = 0; i < 11; ++i) {
        SDL_Rect keyRect = { (int)i * keyWidth + 2, 0, (int)keyWidth - 4, (int)surface->h };
        SDL_FillSurfaceRect(surface, &keyRect, SDL_MapRGB(format, NULL, 255, 255, 255));
        std::string label = whiteKeyLabels[i];
        if (label == "H") {
            label += std::to_string(3 + octaveOffset);
        }
        SDL_Color textColor = { 0, 0, 0, 255 };
        SDL_Surface* tempSurf = TTF_RenderText_Blended(font, label.c_str(), 0, textColor);
        if (tempSurf) {
            SDL_Surface* textSurf = SDL_ConvertSurface(tempSurf, surface->format);
            if (textSurf) {
                SDL_Rect dest = { 
                    i * keyWidth + (keyWidth / 2) - (textSurf->w / 2), 
                    surface->h - 30,
                    textSurf->w, 
                    textSurf->h 
                };
                SDL_SetSurfaceBlendMode(textSurf, SDL_BLENDMODE_BLEND);
                SDL_BlitSurface(textSurf, NULL, surface, &dest);
                SDL_DestroySurface(textSurf);
            }
            SDL_DestroySurface(tempSurf);
        }
    }
    int blackKeyWidth = keyWidth / 2;
    int blackKeyHeight = static_cast<int>(surface->h * 0.6f);
    for (int i = 0; i < 9; ++i) {
        if (i == 2 || i == 6 || i >= 9) continue;
        int xPos = static_cast<int>(i * keyWidth + (keyWidth * 0.75f));
        std::string blackLabel = blackKeyLabels[blackKeyID];
        SDL_Rect blackRect = { xPos, 0, blackKeyWidth, blackKeyHeight };
        SDL_FillSurfaceRect(surface, &blackRect, SDL_MapRGB(format, NULL, 0, 0, 0));
        if (blackKeyID < 7) {
            SDL_Color whiteText = { 255, 255, 255, 255 };
            SDL_Surface* textSurf = TTF_RenderText_Blended(font, blackLabel.c_str(), 0, whiteText);
            if (textSurf) {
                SDL_Rect dest = { 
                    keyWidth * (i + 1) - blackKeyWidth / 4,
                    blackKeyHeight - 30,
                    textSurf->w,
                    textSurf->h
                };
                SDL_SetSurfaceBlendMode(textSurf, SDL_BLENDMODE_BLEND);
                SDL_BlitSurface(textSurf, NULL, surface, &dest);
                SDL_DestroySurface(textSurf);
            }
            ++blackKeyID;
        }
    }
    update[1] = true;
}

void SDL_Manager::renderUI(const std::string& song, const std::string& instrument, int octave) {
    if (windows[0] == nullptr) return;
    int displayW, displayH;
    SDL_GetWindowSize(windows[0], &displayW, &displayH);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(displayW * 0.5f, displayH * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    std::string text = "__________________________________";
    float textWidth = ImGui::CalcTextSize(text.c_str()).x;
    ImGui::SetNextWindowSize(ImVec2(textWidth + 30.0f, 0.0f));
    ImGui::Begin("Music Info", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Current Song: %s", song.c_str());
    ImGui::Text("Instrument: %s", instrument.c_str());
    ImGui::Text("Octave Offset: %d", octave);
    ImGui::Text("\n");
    ImGui::Text("            Controls");
    ImGui::Text("__________________________________");
    ImGui::Text("| Rewind Song:       Left Arrow  |");
    ImGui::Text("| Skip Song:         Right Arrow |");
    ImGui::Text("| Next Song:         Up Arrow    |");
    ImGui::Text("| Previous Song:     Down Arrow  |");
    ImGui::Text("| Pause/Unpause:     Space       |");
    ImGui::Text("| Select Instrument: 1-9         |");
    ImGui::Text("| Open Keyboard:     0           |");
    ImGui::Text("| Increase Octave:   X           |");
    ImGui::Text("| Decrease Octave:   Z           |");
    ImGui::Text("|________________________________|");
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}