cmake_minimum_required(VERSION 3.10)
project(EuphoriaMenu VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Add executable
add_executable(euphoria_menu src/main.cpp)

# Link SFML
target_link_libraries(euphoria_menu sfml-graphics sfml-window sfml-system)

# Include directories
target_include_directories(euphoria_menu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
