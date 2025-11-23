CC = gcc
CFLAGS = -g -O2 -Wall -Wextra -std=c99 -pedantic
LIBS = $(shell pkg-config --cflags --libs sdl2)

SRC = main.c game.c si.c si_font.c si_sdl.c si_menu.c
OBJ = $(SRC:.c=.o)
TARGET = si

CFLAGS += -DNDEBUG

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
