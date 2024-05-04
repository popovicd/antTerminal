# CMakeLists.txt
cmake_minimum_required(VERSION 3.16..3.27) 
project(antTerminal VERSION 0.1 LANGUAGES C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -Og -Wall")

# Set the source and header directories
set(SOURCE_DIR_APP app)
set(SOURCE_DIR_SRC src)

set(INCLUDE_DIR_SRC app)
set(INCLUDE_DIR_SRC src)

# Automatically detect source files
file(
    GLOB SOURCES 
    ${SOURCE_DIR_APP}/*.c
    ${SOURCE_DIR_SRC}/*.c
)

# Automatically detect header files
file(
    GLOB HEADERS 
    ${INCLUDE_DIR_APP}/*.h
    ${INCLUDE_DIR_SRC}/*.h
)

# Create executable target
add_executable(antTerminal ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(antTerminal PUBLIC ${INCLUDE_DIR_APP} ${INCLUDE_DIR_SRC})
