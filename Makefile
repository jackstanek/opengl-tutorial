all: polygons textures

polygons: polygons.o
	gcc -g -lGLEW -lGL -lSDL2 -lm -o polygons polygons.o

polygons.o: polygons.c
	gcc -g -c polygons.c

textures: textures.o
	gcc -g -lGLEW -lGL -lSOIL -lSDL2 -lm -o textures textures.o

textures.o: textures.c
	gcc -g -c textures.c
