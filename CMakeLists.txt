cmake_minimum_required(VERSION 3.12)
project(Algorithms_for_OSM)

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 11)

# Compiler optimization flags
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -march=native -ffast-math -flto=auto")

# Add your source files here
set(SOURCES
    src/main.cpp
    src/t2.cpp
    src/t3.cpp
)

# Find required packages
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(EXPAT REQUIRED)

# Add any additional include directories here
include_directories(include ${ZLIB_INCLUDE_DIRS} ${BZIP2_INCLUDE_DIR} ${EXPAT_INCLUDE_DIRS})

# Add any additional library directories here
link_directories(lib)

# Add any additional libraries here
set(LIBRARIES
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${EXPAT_LIBRARIES}
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Link the libraries
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})