# Prepare compiler
CC := g++
CFLAGS := -Wall -Wextra

# Variables
BUILD_DIR := build
SRCS_DIR := source_files
SRCS := $(wildcard $(SRCS_DIR)/*.cpp)
OBJS := $(patsubst $(SRCS_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TARGET := $(BUILD_DIR)/StubsGenerator.exe

all: ${BUILD_DIR} ${TARGET}

# Create build directory
${BUILD_DIR}:
ifeq ($(OS),Windows_NT)
	if not exist ${BUILD_DIR} mkdir ${BUILD_DIR}
else
	mkdir -p ${BUILD_DIR}
endif

# Linking process 
${TARGET}: ${OBJS}
	$(CC) $(OBJS) -o $(TARGET)
ifeq ($(OS),Windows_NT)
	del /s /q *.o
else
	rm $(OBJS)
endif
	@echo Build process completed
	@echo Executable file is build/StubsGenerator.exe

# Compilation process
${BUILD_DIR}/%.o: $(SRCS_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
ifeq ($(OS),Windows_NT)
	if exist ${BUILD_DIR} rmdir /s /q ${BUILD_DIR}
else
	rm -rf ${BUILD_DIR}
endif