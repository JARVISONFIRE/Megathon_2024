# Makefile for server
server/Makefile:
CC = gcc
CFLAGS = -Wall -I../shared
LDFLAGS = -lraylib -lm

SOURCES = src/server_main.c src/game_logic.c ../shared/networking.c ../shared/game_networking.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = tower_defense_server

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
