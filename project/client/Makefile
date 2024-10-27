# Makefile for client
CC = gcc
CFLAGS = -Wall -Wextra -I/usr/local/include -g
LDFLAGS = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRCS = client_main.c game_render.c networking.c
OBJS = $(SRCS:.c=.o)
TARGET = client

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
