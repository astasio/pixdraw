all: pixidraw.c
	gcc pixidraw.c -o pixidraw -Wall -ggdb -Wno-deprecated-declarations -lm `pkg-config --cflags --libs gtk+-3.0`
