CFLAGS += -std=c99 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wconversion -Werror=incompatible-pointer-types -Werror=implicit-function-declaration
CFLAGS += -Werror=stack-usage=50000
CFLAGS += -g
CFLAGS += -O3

LDFLAGS += -lm

CFLAGS += $(shell pkg-config --cflags sdl2)
LDFLAGS += $(shell pkg-config --libs sdl2)

SRC := $(filter-out src/main.c, $(wildcard src/*.c src/objects/*.c))
OBJ := $(SRC:src/%.c=obj/%.o)
HEADERS := $(wildcard src/*.h src/objects/*.h)

all: 3dthingy

.PHONY: clean
clean:
	git clean -fXd

obj/%.o: src/%.c $(HEADERS)
	mkdir -p $(@D) && $(CC) -c -o $@ $< $(CFLAGS)

3dthingy: src/main.c $(OBJ) $(HEADERS)
	$(CC) $(CFLAGS) $< $(OBJ) -o $@ $(LDFLAGS)
