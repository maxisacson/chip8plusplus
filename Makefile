SDLFLAGS=$(shell sdl2-config --cflags --libs)

all:
	g++ -std=c++11 -O2 -Wall -fPIC -m64 -I. -Iinclude src/chip8.cxx src/c8pp.cxx $(SDLFLAGS) -o c8pp
