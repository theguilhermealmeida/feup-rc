# Makefile to build the project
# NOTE: This file must not be changed.

# Parameters
CC = gcc
CFLAGS = -Wall

SRC = src/
INCLUDE = include/
BIN = bin/

# Targets
.PHONY: all
all: $(BIN)/main 

$(BIN)/main: main.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o bin/download $^ -I$(INCLUDE)
