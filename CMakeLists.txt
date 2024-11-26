cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(MyProject VERSION 1.0)

# Find SDL2
find_package(SDL2 REQUIRED)


find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)


# Include the core and application directories
add_subdirectory(core)
add_subdirectory(application)