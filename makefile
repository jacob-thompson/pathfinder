CC = c++

CFLAGS = -std=c++11 -Wall -Wno-unused-function

SDL_CFLAGS = $(shell pkg-config --cflags sdl2 sdl2_image sdl2_ttf)
SDL_LDFLAGS = $(shell pkg-config --libs sdl2 sdl2_image sdl2_ttf)

CFLAGS += $(SDL_CFLAGS)
LDFLAGS += $(SDL_LDFLAGS)

IDIR = include
_DEPS = node.hh grid.hh pathfinder.hh
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

BDIR = build
_OBJ = pathfinder.o node.o grid.o main.o
OBJ = $(patsubst %, $(BDIR)/%, $(_OBJ))

OUT = pathfinder

.PHONY: clean run

$(BDIR)/%.o: src/%.cc $(DEPS) clean
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

run:
	./$(OUT)

clean:
	rm -f $(OBJ) $(OUT)
