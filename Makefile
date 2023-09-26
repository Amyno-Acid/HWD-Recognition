MAIN := obj/main.o
OBJ := neural_network.o visual.o dataset.o save.o option.o
OBJ := $(patsubst %.o, obj/%.o, $(OBJ))
EXE := bin/main

LIBS := $$(pkg-config --cflags --libs sdl2 SDL2_ttf) -lm

$(EXE): $(MAIN) $(OBJ)
	gcc $^ $(LIBS) -o $@

$(MAIN): src/main.c
	gcc $^ $(LIBS) -c -o $@

$(OBJ): obj/%.o: src/%.c src/%.h
	gcc $< $(LIBS) -c -o $@
