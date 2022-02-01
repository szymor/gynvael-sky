.PHONY: all clean

PROJECT=sky

all: $(PROJECT)

sky: Sky.cpp Vector3D.cpp Vector3D.h
	g++ -o $(PROJECT) `pkg-config --cflags sdl SDL_gfx` Sky.cpp Vector3D.cpp `pkg-config --libs sdl SDL_gfx`

clean:
	rm -rf $(PROJECT)