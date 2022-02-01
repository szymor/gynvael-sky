.PHONY: all clean

PROJECT=sky

all: $(PROJECT)

sky: Sky.cpp Vector3D.cpp Vector3D.h
	g++ -o $(PROJECT) `sdl-config --cflags` Sky.cpp Vector3D.cpp `sdl-config --libs`

clean:
	rm -rf $(PROJECT)