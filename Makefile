# Variables
BUILD_DIR := build
ifeq ($(OS),Windows_NT)
	TARGET_DIR := ${BUILD_DIR}\bin
else
	TARGET_DIR := ${BUILD_DIR}/bin
endif
SRCS_DIR := source_files
UTILITIES_DIR := ${SRCS_DIR}/utilities
SRCS := $(wildcard $(SRCS_DIR)/*.cpp)
UTILITIES_SRCS := $(wildcard $(UTILITIES_DIR)/*.cpp)
OBJS := $(patsubst $(SRCS_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
OBJS += $(patsubst $(UTILITIES_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(UTILITIES_SRCS))
TARGET := $(TARGET_DIR)/StubsGenerator.exe

# Prepare compiler
CC := g++
CFLAGS := -Wall -Wextra -Iinclude_files -Iinclude_files/utilities -Iexternal_libs

all: ${BUILD_DIR} ${TARGET_DIR} ${TARGET}

# Create build directory
${BUILD_DIR}:
ifeq ($(OS),Windows_NT)
	if not exist ${BUILD_DIR} mkdir ${BUILD_DIR} 
else
	mkdir -p ${BUILD_DIR}
endif

${TARGET_DIR}: ${BUILD_DIR}
ifeq ($(OS),Windows_NT)
	if not exist ${TARGET_DIR} mkdir ${TARGET_DIR} 
else
	mkdir -p ${TARGET_DIR}
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
	@echo Executable file is build/bin/StubsGenerator.exe

# Compilation process
${BUILD_DIR}/%.o: $(SRCS_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

${BUILD_DIR}/%.o: $(UTILITIES_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
ifeq ($(OS),Windows_NT)
	if exist ${BUILD_DIR} rmdir /s /q ${BUILD_DIR}
else
	rm -rf ${BUILD_DIR}
endif
