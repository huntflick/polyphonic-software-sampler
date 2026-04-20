all:
	clang++ sdl_manager.cpp SoundSystem.cpp Sound.cpp main.cpp -I./glm -Wl,-rpath,"/Library/Frameworks" -F/Library/Frameworks -framework sdl3 -framework OpenGL -Wall -o application

clean:
	rm application