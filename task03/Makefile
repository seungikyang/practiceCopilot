# Makefile for 작은 도서관 관리 시스템

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LDFLAGS = -lsqlite3

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
DB_DIR = database
TEST_DIR = tests

# Target executable
TARGET = $(BIN_DIR)/library
TEST_TARGET = $(BIN_DIR)/test_book

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/database.c \
       $(SRC_DIR)/book.c \
       $(SRC_DIR)/member.c \
       $(SRC_DIR)/loan.c

# Object files
OBJS = $(OBJ_DIR)/main.o \
       $(OBJ_DIR)/database.o \
       $(OBJ_DIR)/book.o \
       $(OBJ_DIR)/member.o \
       $(OBJ_DIR)/loan.o

# Header files
HEADERS = $(INC_DIR)/database.h \
          $(INC_DIR)/book.h \
          $(INC_DIR)/member.h \
          $(INC_DIR)/loan.h

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@if not exist "$(OBJ_DIR)" mkdir $(OBJ_DIR)
	@if not exist "$(BIN_DIR)" mkdir $(BIN_DIR)
	@if not exist "$(DB_DIR)" mkdir $(DB_DIR)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo.
	@echo ========================================
	@echo Build completed successfully!
	@echo Executable: $(TARGET)
	@echo ========================================
	@echo.

# Compile source files to object files
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/database.o: $(SRC_DIR)/database.c $(INC_DIR)/database.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/database.c -o $(OBJ_DIR)/database.o

$(OBJ_DIR)/book.o: $(SRC_DIR)/book.c $(INC_DIR)/book.h $(INC_DIR)/database.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/book.c -o $(OBJ_DIR)/book.o

$(OBJ_DIR)/member.o: $(SRC_DIR)/member.c $(INC_DIR)/member.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/member.c -o $(OBJ_DIR)/member.o

$(OBJ_DIR)/loan.o: $(SRC_DIR)/loan.c $(INC_DIR)/loan.h $(INC_DIR)/book.h $(INC_DIR)/member.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/loan.c -o $(OBJ_DIR)/loan.o

# Run the program
run: $(TARGET)
	@echo.
	@echo ========================================
	@echo Running Library Management System...
	@echo ========================================
	@echo.
	$(TARGET)

# Clean build artifacts
clean:
	@if exist "$(OBJ_DIR)" rmdir /s /q $(OBJ_DIR)
	@if exist "$(BIN_DIR)" rmdir /s /q $(BIN_DIR)
	@echo.
	@echo Clean completed!
	@echo.

# Clean everything including database
cleanall: clean
	@if exist "$(DB_DIR)" rmdir /s /q $(DB_DIR)
	@echo.
	@echo Clean all completed (including database)!
	@echo.

# Rebuild from scratch
rebuild: clean all

# Install dependencies (for reference)
install-deps:
	@echo.
	@echo ========================================
	@echo SQLite3 library is required
	@echo ========================================
	@echo For MSYS2/MinGW:
	@echo   pacman -S mingw-w64-ucrt-x86_64-sqlite3
	@echo.
	@echo For Ubuntu/Debian:
	@echo   sudo apt-get install libsqlite3-dev
	@echo.
	@echo For macOS:
	@echo   brew install sqlite3
	@echo.

# Help target
help:
	@echo.
	@echo ========================================
	@echo Available Makefile targets:
	@echo ========================================
	@echo   make          - Build the project
	@echo   make all      - Build the project
	@echo   make run      - Build and run the program
	@echo   make test     - Build and run unit tests
	@echo   make clean    - Remove object and binary files
	@echo   make cleanall - Remove all generated files including database
	@echo   make rebuild  - Clean and rebuild from scratch
	@echo   make install-deps - Show how to install dependencies
	@echo   make help     - Show this help message
	@echo ========================================
	@echo.

# Test source files
TEST_SRCS = $(TEST_DIR)/test_book.c

# Test object files (only need non-main objects)
TEST_OBJS = $(OBJ_DIR)/database.o \
            $(OBJ_DIR)/book.o

# Build and run tests
test: directories $(TEST_TARGET)
	@echo.
	@echo ========================================
	@echo Running tests...
	@echo ========================================
	@echo.
	$(TEST_TARGET)

# Build test executable
$(TEST_TARGET): $(TEST_OBJS) $(TEST_SRCS)
	$(CC) $(CFLAGS) $(TEST_SRCS) $(TEST_OBJS) -o $(TEST_TARGET) $(LDFLAGS)
	@echo.
	@echo Test executable built: $(TEST_TARGET)
	@echo.

# Phony targets
.PHONY: all directories clean cleanall rebuild run install-deps help test
