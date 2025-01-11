ifeq (run, $(firstword $(MAKECMDGOALS)))
  # get arguments for `make run`
  RUN_ARGS := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

CC = g++

CFLAGS = -I.
CFLAGS += -std=c++11
CFLAGS += -Wall

SDLFLAGS = `sdl2-config --libs --cflags`
SDLFLAGS += -lSDL2_ttf

ifeq ($(shell uname), Darwin)
CFLAGS += -Wno-unused-command-line-argument
SDLFLAGS += -I/opt/homebrew/Cellar/sdl2/*/include
endif

IDIR = include
_DEPS = node.hh grid.hh pathfinder.hh
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

BDIR = build
_OBJ = pathfinder.o node.o grid.o main.o
OBJ = $(patsubst %, $(BDIR)/%, $(_OBJ))

OUT = pathfinder

.PHONY: clean run

$(BDIR)/%.o: src/%.cc $(DEPS) clean
	$(CC) -c -o $@ $< $(CFLAGS) $(SDLFLAGS)

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(SDLFLAGS)

run:
	./$(OUT) $(RUN_ARGS)

clean:
	rm -f $(OBJ) $(OUT)
