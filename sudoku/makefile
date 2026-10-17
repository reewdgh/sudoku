CC = gcc
CFLAGS = -mwindows -lwinmm -Wall -Wextra -Wpedantic -Werror -Wformat=2 

TARGET = main.exe
SRC = src/main.c src/sudoku_gui.c src/sudoku_game.c
run:.
	./$(TARGET)
	
compile:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)
	./$(TARGET)

