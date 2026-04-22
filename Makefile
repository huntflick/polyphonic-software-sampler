all:
	clang++ ImGui/imgui.cpp ImGui/imgui_draw.cpp ImGui/imgui_widgets.cpp ImGui/imgui_tables.cpp ImGui/backends/imgui_impl_sdl3.cpp ImGui/backends/imgui_impl_opengl3.cpp sdl_manager.cpp SoundSystem.cpp Sound.cpp main.cpp -I./ImGui -I./ImGui/backends -I./glm -Wl,-rpath,"/Library/Frameworks" -F/Library/Frameworks -framework sdl3 -framework SDL3_ttf -framework OpenGL -Wall -std=c++17 -o application

clean:
	rm application