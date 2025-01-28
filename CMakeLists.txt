# Minimum CMake version required
cmake_minimum_required(VERSION 3.15)

# Define the project name and language
project(mandebrot_render LANGUAGES C)

# Find the Raylib package (requires at least version 3.0)
find_package(raylib 3.0 REQUIRED)
find_package(OpenMP)


# Set the C standard to C11
set(CMAKE_C_STANDARD 11)

# Add the main source file to create an executable
add_executable(${PROJECT_NAME} src/main.c)

# Link the Raylib library to the project
target_link_libraries(${PROJECT_NAME} PRIVATE raylib m)

if(OpenMP_C_FLAGS)
    target_compile_options(${PROJECT_NAME} PRIVATE ${OpenMP_C_FLAGS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenMP_C_LIBRARIES})
endif()
