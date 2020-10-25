# Make the game.
CC 		   = gcc
CCFLAGS    = -Wall -Werror -Wpedantic -Wextra --std=c++20 -O0 \
			 $(shell pkgconf sdl2 --cflags)
LDFLAGS    = -lm -lSDL2_image -lSDL2_ttf -lpthread -lstdc++ \
			 $(shell pkgconf sdl2 --libs)
DEBUG_INFO = no

ifeq ($(DEBUG_INFO), yes)
	CCFLAGS += -g
endif

BIN       = breakout
BIN_FLAGS = -print_fps=true
SRCS      = main.cc context.cc game.cc shapes.cc ui.cc utils.cc
OBJS      = $(SRCS:.cc=.o)
DEPS 	  = $(SRCS:.cc=.d)

.PHONY: all clean test leaks

all: $(BIN)

$(BIN): $(OBJS)
	ctags -R
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $@ $^

-include $(DEPS)

%.o: %.cc Makefile
	$(CC) $(CCFLAGS) -MMD -MP -c -o $@ $<

test: $(BIN)
	./$< $(BIN_FLAGS)

leaks: $(BIN)
	valgrind -s --leak-check=full --show-leak-kinds=all ./$<

clean:
	rm -f *.o *.d $(BIN)
