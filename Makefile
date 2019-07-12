# sudo apt install libsdl2-dev

CFLAGS += -std=c99 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wconversion -Werror=incompatible-pointer-types -Werror=implicit-function-declaration
#CFLAGS += -Werror=stack-usage=16384   # BUFSIZ is 8192 on my system
CFLAGS += -g
#CFLAGS += -O3
LDFLAGS += -lm

CFLAGS += $(shell pkg-config --cflags sdl2)
LDFLAGS += $(shell pkg-config --libs sdl2)

all: hello

.PHONY: clean
clean:
	rm -f hello
